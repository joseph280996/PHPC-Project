#include <chrono>
#include <filesystem>
#include <iostream>
#include <mpi.h>
#include <opencv2/opencv.hpp>
#include <string>
#include <unistd.h>

using namespace cv;
using namespace std;
using namespace std::chrono;
namespace fs = std::filesystem;

// Clamp value between 0 and 255
inline uchar clamp_color(int val) {
  if (val < 0)
    val = 0;
  if (val > 255)
    val = 255;
  return (uchar)val;
}

void increase_channels_sequential(Mat &image, int red_inc, int green_inc,
                                  int blue_inc) {
  int rows = image.rows;
  int cols = image.cols;
  int channels = image.channels();

  // OpenCV default is BGR order:
  // channel 0: Blue, channel 1: Green, channel 2: Red (for a 3-channel image)
  for (int r = 0; r < rows; r++) {
    uchar *row = image.ptr(r);
    for (int c = 0; c < cols; c++) {
      int base_idx = c * channels;
      int blue_val = row[base_idx + 0] + blue_inc;
      int green_val = row[base_idx + 1] + green_inc;
      int red_val = row[base_idx + 2] + red_inc;

      row[base_idx + 0] = clamp_color(blue_val);
      row[base_idx + 1] = clamp_color(green_val);
      row[base_idx + 2] = clamp_color(red_val);
    }
  }
}

void increase_channels_parallel(uchar *shared_data, int rows, int cols,
                                int channels, int rank, int num_processes,
                                int red_inc, int green_inc, int blue_inc) {
  int block_size = rows / num_processes;
  int start_row = rank * block_size;
  int end_row = (rank == num_processes - 1) ? rows : (rank + 1) * block_size;

  for (int r = start_row; r < end_row; r++) {
    for (int c = 0; c < cols; c++) {
      int base_idx = (r * cols + c) * channels;
      int blue_val = shared_data[base_idx + 0] + blue_inc;
      int green_val = shared_data[base_idx + 1] + green_inc;
      int red_val = shared_data[base_idx + 2] + red_inc;

      shared_data[base_idx + 0] = clamp_color(blue_val);
      shared_data[base_idx + 1] = clamp_color(green_val);
      shared_data[base_idx + 2] = clamp_color(red_val);
    }
  }
}

int main(int argc, char **argv) {
  if (argc != 6) {
    cout << "Usage: " << argv[0]
         << " <image_path> <red_inc> <green_inc> <blue_inc> "
            "<with_sequential_flag>"
         << endl;
    cout << "Example: " << argv[0] << " input.jpg 50 0 0" << endl
         << "This would add 50 to the red channel." << endl;
    return -1;
  }

  string image_path = argv[1];
  int red_inc = stoi(argv[2]);
  int green_inc = stoi(argv[3]);
  int blue_inc = stoi(argv[4]);

  MPI_Init(&argc, &argv);

  int rank, num_processes;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &num_processes);

  MPI_Comm nodeComm;
  MPI_Comm_split_type(MPI_COMM_WORLD, MPI_COMM_TYPE_SHARED, rank, MPI_INFO_NULL,
                      &nodeComm);

  // Variables for shared memory
  MPI_Win win;
  uchar *sharedData;
  int dims[3] = {0, 0, 0};

  if (rank == 0) {
    // Read image and do sequential version
    Mat image = imread(image_path);
    if (image.empty()) {
      cout << "Error: Could not read the image." << endl;
      MPI_Abort(MPI_COMM_WORLD, 1);
      return -1;
    }

    // Do sequential version
    const string with_sequential_flag = argv[5];
    if (with_sequential_flag == "true") {
      Mat seqImage = image.clone();
      auto start = high_resolution_clock::now();
      increase_channels_sequential(seqImage, red_inc, green_inc, blue_inc);
      auto stop = high_resolution_clock::now();
      cout << "Sequential time: "
           << duration_cast<microseconds>(stop - start).count()
           << " microseconds" << endl;

      const char *output_dir = std::getenv("SEQ_OUTPUT_DIR");
      string seq_out_path = string(output_dir) + "/sequential_color_result.jpg";
      bool success = imwrite(seq_out_path, seqImage);
      if (!success) {
        cout << "Error: Could not write " << seq_out_path << endl;
      }
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
    Mat image = imread(image_path);
    memcpy(sharedData, image.data, total_image_size);
  }

  // Ensure all processes see the initial data
  MPI_Barrier(MPI_COMM_WORLD);

  // Start parallel timing
  auto start = high_resolution_clock::now();

  // Each process increases its portion of red, green, and blue channels
  increase_channels_parallel(sharedData, dims[0], dims[1], dims[2], rank,
                             num_processes, red_inc, green_inc, blue_inc);

  // Wait for all processes to complete
  MPI_Barrier(MPI_COMM_WORLD);

  if (rank == 0) {
    auto stop = high_resolution_clock::now();
    cout << "Parallel time: "
         << duration_cast<microseconds>(stop - start).count() << " microseconds"
         << endl;

    // Save result
    Mat result(dims[0], dims[1], CV_8UC(dims[2]), sharedData);
    const char *output_dir = std::getenv("PAR_OUTPUT_DIR");
    string parallel_output =
        string(output_dir) +
        "/parallel_color_result.jpg"; // Also fixed "sequential" to "parallel"
                                      // in the filename
    bool success =
        imwrite(parallel_output, result); // Changed seqImage to result
    if (!success) {
      cout << "Error: Could not write " << parallel_output << endl;
    }
  }

  MPI_Win_free(&win);
  MPI_Comm_free(&nodeComm);
  MPI_Finalize();
  return 0;
}
