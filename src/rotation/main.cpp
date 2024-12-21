#include <chrono>
#include <iostream>
#include <mpi.h>
#include <opencv2/opencv.hpp>
#include <string>

using namespace cv;
using namespace std;
using namespace std::chrono;

enum ROTATIONTYPE { CLOCKWISE = 0, COUNTERCLOCKWISE = 1 };

ROTATIONTYPE parseRotationType(const string &type) {
  if (type == "c" || type == "clockwise") {
    return CLOCKWISE;
  } else if (type == "cc" || type == "counterclockwise") {
    return COUNTERCLOCKWISE;
  } else {
    throw invalid_argument("Invalid rotation type. Use 'c'/'clockwise' or "
                           "'cc'/'counterclockwise'");
  }
}

void rotate_clockwise_sequential(const Mat &input, Mat &output) {
  // For clockwise rotation of 90 degrees:
  // (r, c) -> (c, new_cols - 1 - r)
  // new_rows = input.cols, new_cols = input.rows
  int rows = input.rows;
  int cols = input.cols;
  int channels = input.channels();

  output.create(cols, rows, input.type());

  for (int r = 0; r < rows; r++) {
    const uchar *in_row = input.ptr<uchar>(r);
    for (int c = 0; c < cols; c++) {
      for (int ch = 0; ch < channels; ch++) {
        output.at<Vec3b>(c, rows - 1 - r)[ch] = in_row[c * channels + ch];
      }
    }
  }
}

void rotate_counterclockwise_sequential(const Mat &input, Mat &output) {
  // For counterclockwise rotation of 90 degrees:
  // (r, c) -> (new_rows - 1 - c, r)
  // new_rows = input.cols, new_cols = input.rows
  int rows = input.rows;
  int cols = input.cols;
  int channels = input.channels();

  output.create(cols, rows, input.type());

  for (int r = 0; r < rows; r++) {
    const uchar *in_row = input.ptr<uchar>(r);
    for (int c = 0; c < cols; c++) {
      for (int ch = 0; ch < channels; ch++) {
        output.at<Vec3b>(cols - 1 - c, r)[ch] = in_row[c * channels + ch];
      }
    }
  }
}

void rotate_parallel_clockwise(const uchar *input_data, uchar *output_data,
                               int in_rows, int in_cols, int channels, int rank,
                               int num_processes) {
  // Parallel partitioning: distribute rows of the input image among processes
  // Each process handles a subset of rows from the input image.
  // For a clockwise rotation:
  // (r, c) -> (c, out_cols - 1 - r)
  // out_rows = in_cols, out_cols = in_rows
  int out_rows = in_cols;
  int out_cols = in_rows;

  int block_size = in_rows / num_processes;
  int start_row = rank * block_size;
  int end_row = (rank == num_processes - 1) ? in_rows : (rank + 1) * block_size;

  for (int r = start_row; r < end_row; r++) {
    for (int c = 0; c < in_cols; c++) {
      for (int ch = 0; ch < channels; ch++) {
        int in_index = (r * in_cols + c) * channels + ch;
        int out_r = c;
        int out_c = out_cols - 1 - r;
        int out_index = (out_r * out_cols + out_c) * channels + ch;
        output_data[out_index] = input_data[in_index];
      }
    }
  }
}

void rotate_parallel_counterclockwise(const uchar *input_data,
                                      uchar *output_data, int in_rows,
                                      int in_cols, int channels, int rank,
                                      int num_processes) {
  // For counterclockwise rotation:
  // (r, c) -> (out_rows - 1 - c, r)
  // out_rows = in_cols, out_cols = in_rows
  int out_rows = in_cols;
  int out_cols = in_rows;

  int block_size = in_rows / num_processes;
  int start_row = rank * block_size;
  int end_row = (rank == num_processes - 1) ? in_rows : (rank + 1) * block_size;

  for (int r = start_row; r < end_row; r++) {
    for (int c = 0; c < in_cols; c++) {
      for (int ch = 0; ch < channels; ch++) {
        int in_index = (r * in_cols + c) * channels + ch;
        int out_r = out_rows - 1 - c;
        int out_c = r;
        int out_index = (out_r * out_cols + out_c) * channels + ch;
        output_data[out_index] = input_data[in_index];
      }
    }
  }
}

int main(int argc, char **argv) {
  if (argc != 4) {
    cout << "Usage: " << argv[0] << " <image_path> <rotation_type> <with_sequential_flag>" << endl;
    cout << "rotation_type: 'c' or 'clockwise' for clockwise rotation" << endl;
    cout << "               'cc' or 'counterclockwise' for counterclockwise "
            "rotation"
         << endl;
    return -1;
  }

  ROTATIONTYPE rotationtype;
  try {
    rotationtype = parseRotationType(argv[2]);
  } catch (const invalid_argument &e) {
    cout << "Error: " << e.what() << endl;
    return -1;
  }

  MPI_Init(&argc, &argv);

  int rank, num_processes;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &num_processes);

  // Create shared communicator
  MPI_Comm nodeComm;
  MPI_Comm_split_type(MPI_COMM_WORLD, MPI_COMM_TYPE_SHARED, rank, MPI_INFO_NULL,
                      &nodeComm);

  // We'll need two shared memory regions:
  // 1) For the input image data
  // 2) For the output (rotated) image data

  MPI_Win in_win, out_win;
  uchar *sharedInData = nullptr;
  uchar *sharedOutData = nullptr;

  int in_dims[3] = {0, 0, 0}; // rows, cols, channels
  int out_dims[3] = {0, 0, 0};

  if (rank == 0) {
    // Read image
    Mat input = imread(argv[1]);
    if (input.empty()) {
      cout << "Error: Could not read the image." << endl;
      MPI_Abort(MPI_COMM_WORLD, 1);
      return -1;
    }

    // Compute sequential rotation
    Mat seqOutput;
    auto start_seq = high_resolution_clock::now();
    const string with_sequential_flag = argv[3];
    if (with_sequential_flag == "true") {
      if (rotationtype == CLOCKWISE) {
        rotate_clockwise_sequential(input, seqOutput);
      } else {
        rotate_counterclockwise_sequential(input, seqOutput);
      }
      auto stop_seq = high_resolution_clock::now();
      cout << "Sequential time: "
           << duration_cast<microseconds>(stop_seq - start_seq).count()
           << " microseconds" << endl;

      string rot_str =
          (rotationtype == CLOCKWISE) ? "clockwise" : "counterclockwise";
      const char *output_dir = std::getenv("SEQ_OUTPUT_DIR");
      std::string outputPath =
          std::string(output_dir) + "/sequential_" + rot_str + "_result.jpg";
      cv::imwrite(outputPath, seqOutput);
    }

    // Set dimensions
    in_dims[0] = input.rows;
    in_dims[1] = input.cols;
    in_dims[2] = input.channels();

    // After rotation:
    // out_rows = in_cols, out_cols = in_rows
    out_dims[0] = in_dims[1]; // out rows
    out_dims[1] = in_dims[0]; // out cols
    out_dims[2] = in_dims[2];
  }
  // Make sure everyone has the dims
  MPI_Bcast(in_dims, 3, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(out_dims, 3, MPI_INT, 0, MPI_COMM_WORLD);
  // We'll allocate shared memory for input image and copy data
  // We'll also allocate shared memory for output image
  size_t in_total_size = (rank == 0) ? in_dims[0] * in_dims[1] * in_dims[2] : 0;
  size_t out_total_size =
      (rank == 0) ? out_dims[0] * out_dims[1] * out_dims[2] : 0;

  MPI_Win_allocate_shared(in_total_size, 1, MPI_INFO_NULL, nodeComm,
                          &sharedInData, &in_win);
  MPI_Win_allocate_shared(out_total_size, 1, MPI_INFO_NULL, nodeComm,
                          &sharedOutData, &out_win);

  if (rank != 0) {
    MPI_Aint in_shared_size, out_shared_size;
    int in_disp_unit, out_disp_unit;
    MPI_Win_shared_query(in_win, 0, &in_shared_size, &in_disp_unit,
                         &sharedInData);
    MPI_Win_shared_query(out_win, 0, &out_shared_size, &out_disp_unit,
                         &sharedOutData);
  }

  if (rank == 0) {
    Mat image = imread(argv[1]);

    memcpy(sharedInData, image.data, in_total_size);
  }

  int in_rows = in_dims[0];
  int in_cols = in_dims[1];
  int in_ch = in_dims[2];

  // Ensure all processes see the initial data
  MPI_Barrier(MPI_COMM_WORLD);

  // Start parallel timing
  auto start_par = high_resolution_clock::now();

  // Perform parallel rotation
  if (rotationtype == CLOCKWISE) {
    rotate_parallel_clockwise(sharedInData, sharedOutData, in_rows, in_cols,
                              in_ch, rank, num_processes);
  } else {
    rotate_parallel_counterclockwise(sharedInData, sharedOutData, in_rows,
                                     in_cols, in_ch, rank, num_processes);
  }

  MPI_Barrier(MPI_COMM_WORLD);

  if (rank == 0) {
    auto stop_par = high_resolution_clock::now();
    cout << "Parallel time: "
         << duration_cast<microseconds>(stop_par - start_par).count()
         << " microseconds" << endl;

    // out_dims were computed: out_rows = in_cols, out_cols = in_rows

    Mat result(out_dims[0], out_dims[1], CV_8UC(out_dims[2]), sharedOutData);
    string rot_str =
        (rotationtype == CLOCKWISE) ? "clockwise" : "counterclockwise";
    const char *output_dir = std::getenv("PAR_OUTPUT_DIR");
    std::string outputPath =
        std::string(output_dir) + "/parallel_" + rot_str + "_result.jpg";
    cv::imwrite(outputPath, result);
  }

  MPI_Win_free(&in_win);
  MPI_Win_free(&out_win);
  MPI_Comm_free(&nodeComm);
  MPI_Finalize();
  return 0;
}
