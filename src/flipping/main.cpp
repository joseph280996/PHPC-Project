#include <chrono>
#include <iostream>
#include <mpi.h>
#include <opencv2/opencv.hpp>
#include <string>

using namespace cv;
using namespace std;
using namespace std::chrono;

enum FlipType { HORIZONTAL = 0, VERTICAL = 1 };

void flip_horizontal_sequential(Mat &image) {
  int rows = image.rows;
  int cols = image.cols;
  int channels = image.channels();

  // Swap rows from top and bottom
  for (int i = 0; i < rows / 2; i++) {
    uchar *top_row = image.ptr(i);
    uchar *bottom_row = image.ptr(rows - 1 - i);

    vector<uchar> temp(cols * channels);
    memcpy(temp.data(), top_row, cols * channels);
    memcpy(top_row, bottom_row, cols * channels);
    memcpy(bottom_row, temp.data(), cols * channels);
  }
}

void flip_vertical_sequential(Mat &image) {
  int rows = image.rows;
  int cols = image.cols;
  int channels = image.channels();

  for (int i = 0; i < rows; i++) {
    uchar *row = image.ptr(i);
    for (int j = 0; j < cols / 2; j++) {
      for (int c = 0; c < channels; c++) {
        swap(row[j * channels + c], row[(cols - 1 - j) * channels + c]);
      }
    }
  }
}

void flip_vertical_parallel(uchar *shared_data, int rows, int cols,
                            int channels, int rank, int num_processes) {
  int block_size = rows / num_processes;
  int start_row = rank * block_size;
  int end_row = (rank == num_processes - 1) ? rows : (rank + 1) * block_size;

  for (int i = start_row; i < end_row; i++) {
    for (int j = 0; j < cols / 2; j++) {
      for (int c = 0; c < channels; c++) {
        int left_idx = (i * cols + j) * channels + c;
        int right_idx = (i * cols + (cols - 1 - j)) * channels + c;
        swap(shared_data[left_idx], shared_data[right_idx]);
      }
    }
  }
}

void flip_horizontal_parallel(uchar *shared_data, int rows, int cols,
                              int channels, int rank, int num_processes) {
  // Calculate work on half the rows since we only need to process half
  int half_rows = rows / 2;
  int block_size = half_rows / num_processes;
  int start_row = rank * block_size;
  int end_row =
      (rank == num_processes - 1) ? half_rows : (rank + 1) * block_size;

  // Now only process rows in the top half
  for (int i = start_row; i < end_row; i++) {
    int corresponding_row = rows - 1 - i;
    size_t current_row_offset = i * cols * channels;
    size_t opposite_row_offset = corresponding_row * cols * channels;

    vector<uchar> temp(cols * channels);
    memcpy(temp.data(), &shared_data[current_row_offset], cols * channels);
    memcpy(&shared_data[current_row_offset], &shared_data[opposite_row_offset],
           cols * channels);
    memcpy(&shared_data[opposite_row_offset], temp.data(), cols * channels);
  }
}

FlipType parseFlipType(const string &type) {
  if (type == "h" || type == "horizontal") {
    return HORIZONTAL;
  } else if (type == "v" || type == "vertical") {
    return VERTICAL;
  } else {
    throw invalid_argument(
        "Invalid flip type. Use 'h'/'horizontal' or 'v'/'vertical'");
  }
}

int main(int argc, char **argv) {
  if (argc != 4) {
    cout << "Usage: " << argv[0]
         << " <image_path> <flip_type> <with_sequential>" << endl;
    cout << "flip_type: 'h' or 'horizontal' for horizontal flip" << endl;
    cout << "          'v' or 'vertical' for vertical flip" << endl;
    return -1;
  }

  // Parse flip type before MPI init in case of error
  FlipType flip_type;
  try {
    flip_type = parseFlipType(argv[2]);
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

  // Variables for shared memory
  MPI_Win win;
  uchar *sharedData;
  int dims[3] = {0, 0, 0}; // rows, cols, channels

  if (rank == 0) {
    // Read image and do sequential version
    Mat image = imread(argv[1]);
    if (image.empty()) {
      cout << "Error: Could not read the image." << endl;
      MPI_Abort(MPI_COMM_WORLD, 1);
      return -1;
    }

    // Do sequential version
    const string with_sequential_flag = argv[3];
    if (with_sequential_flag == "true") {
      Mat seqImage = image.clone();
      auto start = high_resolution_clock::now();
      if (flip_type == HORIZONTAL) {
        flip_horizontal_sequential(seqImage);
      } else { // VERTICAL
        flip_vertical_sequential(seqImage);
      }
      auto stop = high_resolution_clock::now();
      cout << "Sequential time: "
           << duration_cast<microseconds>(stop - start).count()
           << " microseconds" << endl;
      string flip_str = (flip_type == HORIZONTAL) ? "horizontal" : "vertical";
      const char *output_dir = std::getenv("SEQ_OUTPUT_DIR");
      string sequential_output =
          string(output_dir) + "/sequential_" + flip_str + "_result.jpg";
      imwrite(sequential_output, seqImage);
    }

    // Share dimensions
    dims[0] = image.rows;
    dims[1] = image.cols;
    dims[2] = image.channels();
  }

  // Broadcast dimensions to all processes
  MPI_Bcast(dims, 3, MPI_INT, 0, MPI_COMM_WORLD);

  // Allocate shared memory - only root allocates real size
  size_t total_image_size = (rank == 0) ? dims[0] * dims[1] * dims[2] : 0;
  MPI_Win_allocate_shared(total_image_size, 1, MPI_INFO_NULL, nodeComm,
                          &sharedData, &win);

  // Non-root processes get pointer to root's memory
  if (rank != 0) {
    MPI_Aint shared_memory_size;
    int disp_unit;
    MPI_Win_shared_query(win, 0, &shared_memory_size, &disp_unit, &sharedData);
  }

  // Root copies image data to shared memory
  if (rank == 0) {
    Mat image = imread(argv[1]);
    memcpy(sharedData, image.data, total_image_size);
  }

  // Ensure all processes see the initial data
  MPI_Barrier(MPI_COMM_WORLD);

  // Start parallel timing
  auto start = high_resolution_clock::now();

  // Each process flips its portion
  if (flip_type == HORIZONTAL) {
    // Process assigned rows if they need flipping
    flip_horizontal_parallel(sharedData, dims[0], dims[1], dims[2], rank,
                             num_processes);
  } else { // VERTICAL
    flip_vertical_parallel(sharedData, dims[0], dims[1], dims[2], rank,
                           num_processes);
  }

  // Wait for all processes to complete
  MPI_Barrier(MPI_COMM_WORLD);

  if (rank == 0) {
    auto stop = high_resolution_clock::now();
    cout << "Parallel time: "
         << duration_cast<microseconds>(stop - start).count() << " microseconds"
         << endl;

    // Save result
    Mat result(dims[0], dims[1], CV_8UC(dims[2]), sharedData);
    string flip_str = (flip_type == HORIZONTAL) ? "horizontal" : "vertical";

    const char *output_dir = std::getenv("PAR_OUTPUT_DIR");
    string parallel_output =
        string(output_dir) + "/parallel_" + flip_str +
        "_result.jpg"; // Also fixed "sequential" to "parallel" in the filename
    imwrite(parallel_output, result); // Changed seqImage to result
  }

  MPI_Win_free(&win);
  MPI_Comm_free(&nodeComm);
  MPI_Finalize();
  return 0;
}
