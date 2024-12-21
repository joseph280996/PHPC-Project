#include <chrono>
#include <cmath>
#include <complex>
#include <iomanip>
#include <mpi.h>
#include <opencv2/opencv.hpp>
#include <vector>

const double PI = 3.14159265358979323846;
using Complex = std::complex<double>;

// 1D FFT implementation
// Pure forward 1D FFT implementation
std::vector<Complex> fft1D(std::vector<Complex> &x) {
  int n = x.size();
  if (n <= 1)
    return x;

  // Split into even and odd
  std::vector<Complex> even(n / 2), odd(n / 2);
  for (int i = 0; i < n / 2; i++) {
    even[i] = x[2 * i];
    odd[i] = x[2 * i + 1];
  }

  // Recursive FFT
  even = fft1D(even);
  odd = fft1D(odd);

  // Combine
  std::vector<Complex> result(n);
  Complex w = 1;
  Complex wn = std::polar(1.0, -2 * PI / n); // Only forward FFT

  for (int i = 0; i < n / 2; i++) {
    result[i] = even[i] + w * odd[i];
    result[i + n / 2] = even[i] - w * odd[i];
    w *= wn;
  }

  return result;
}

class PencilFFT {
private:
  int rank, size;
  int rows, cols;
  int local_rows;
  MPI_Comm comm;
  std::vector<Complex> local_data;

  void debugPrint(const std::string &message) {
    MPI_Barrier(comm); // Synchronize for cleaner output
    if (rank == 0)
      std::cout << "\n=== " << message << " ===" << std::endl;
    for (int i = 0; i < size; i++) {
      MPI_Barrier(comm);
      if (rank == i) {
        std::cout << "Process " << rank << ": "
                  << "local_rows=" << local_rows << ", cols=" << cols
                  << ", data_size=" << local_data.size() << std::endl;
      }
    }
    MPI_Barrier(comm);
  }

public:
  PencilFFT(int total_rows, int total_cols, MPI_Comm comm = MPI_COMM_WORLD)
      : rows(total_rows), cols(total_cols), comm(comm) {
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    // Calculate pencil width (local_rows)
    local_rows = (rows + size - 1) / size;
    if (rank == size - 1) {
      // Last process might get fewer rows
      local_rows = rows - (size - 1) * local_rows;
      if (local_rows <= 0)
        local_rows = 0;
    }
    local_data.resize(local_rows * cols);

    debugPrint("Constructor");
  }

  void distributeData(const cv::Mat &channel) {
    // Create pencil distribution
    int base_rows = rows / size;
    int my_rows = (rank == size - 1) ? rows - rank * base_rows : base_rows;
    local_rows = my_rows;
    local_data.resize(local_rows * cols);

    // Root distributes data
    if (rank == 0) {
      // Copy own portion
      for (int i = 0; i < local_rows; i++) {
        for (int j = 0; j < cols; j++) {
          local_data[i * cols + j] = Complex(channel.at<uchar>(i, j), 0);
        }
      }

      // Send to other processes
      for (int p = 1; p < size; p++) {
        int p_rows = (p == size - 1) ? rows - p * base_rows : base_rows;
        std::vector<Complex> temp_buffer(p_rows * cols);

        for (int i = 0; i < p_rows; i++) {
          for (int j = 0; j < cols; j++) {
            temp_buffer[i * cols + j] =
                Complex(channel.at<uchar>(p * base_rows + i, j), 0);
          }
        }

        MPI_Send(temp_buffer.data(), p_rows * cols, MPI_DOUBLE_COMPLEX, p, 0,
                 comm);
      }
    } else {
      // Receive data
      MPI_Status status;
      MPI_Recv(local_data.data(), local_rows * cols, MPI_DOUBLE_COMPLEX, 0, 0,
               comm, &status);
    }
  }

  void computeFFT() {
    if (rank == 0) {
      std::cout << "Starting FFT computation" << std::endl;
    }

    // Debug: Print first few values before any operation
    if (rank == 0) {
      std::cout << "Initial values (first 5x5):" << std::endl;
      for (int i = 0; i < std::min(5, local_rows); i++) {
        for (int j = 0; j < std::min(5, cols); j++) {
          std::cout << std::abs(local_data[i * cols + j]) << " ";
        }
        std::cout << std::endl;
      }
    }

    // 1. Row-wise FFT
    for (int i = 0; i < local_rows; i++) {
      std::vector<Complex> row(cols);
      for (int j = 0; j < cols; j++) {
        row[j] = local_data[i * cols + j];
      }
      row = fft1D(row);
      for (int j = 0; j < cols; j++) {
        local_data[i * cols + j] = row[j];
      }
    }

    // Debug: Print after row FFT
    if (rank == 0) {
      std::cout << "\nAfter row FFT (first 5x5):" << std::endl;
      for (int i = 0; i < std::min(5, local_rows); i++) {
        for (int j = 0; j < std::min(5, cols); j++) {
          std::cout << std::abs(local_data[i * cols + j]) << " ";
        }
        std::cout << std::endl;
      }
    }

    // 2. Transpose
    int old_rows = local_rows;
    int old_cols = cols;
    transposeGlobal();

    // Debug: Print after transpose
    if (rank == 0) {
      std::cout << "\nAfter transpose (first 5x5):" << std::endl;
      for (int i = 0; i < std::min(5, local_rows); i++) {
        for (int j = 0; j < std::min(5, cols); j++) {
          std::cout << std::abs(local_data[i * cols + j]) << " ";
        }
        std::cout << std::endl;
      }
      std::cout << "New dimensions: " << local_rows << "x" << cols << " (was "
                << old_rows << "x" << old_cols << ")" << std::endl;
    }

    // 3. Column-wise FFT (now row-wise after transpose)
    for (int i = 0; i < local_rows; i++) {
      std::vector<Complex> row(cols);
      for (int j = 0; j < cols; j++) {
        row[j] = local_data[i * cols + j];
      }
      row = fft1D(row);
      for (int j = 0; j < cols; j++) {
        local_data[i * cols + j] = row[j];
      }
    }

    // Debug: Print after column FFT
    if (rank == 0) {
      std::cout << "\nAfter column FFT (first 5x5):" << std::endl;
      for (int i = 0; i < std::min(5, local_rows); i++) {
        for (int j = 0; j < std::min(5, cols); j++) {
          std::cout << std::abs(local_data[i * cols + j]) << " ";
        }
        std::cout << std::endl;
      }
    }

    // 4. Transpose back
    transposeGlobal();

    // Debug: Print final result
    if (rank == 0) {
      std::cout << "\nFinal result (first 5x5):" << std::endl;
      for (int i = 0; i < std::min(5, local_rows); i++) {
        for (int j = 0; j < std::min(5, cols); j++) {
          std::cout << std::abs(local_data[i * cols + j]) << " ";
        }
        std::cout << std::endl;
      }
    }
  }

  void transposeGlobal() {
    if (rank == 0) {
      std::cout << "Starting sequential transpose..." << std::endl;
    }

    double trans_start = MPI_Wtime();

    // Find the maximum data size across all processes
    int my_data_size = local_rows * cols;
    int max_data_size;
    MPI_Allreduce(&my_data_size, &max_data_size, 1, MPI_INT, MPI_MAX, comm);

    // Pad local data to max size
    std::vector<Complex> padded_local = local_data;
    padded_local.resize(max_data_size, Complex(0, 0));

    // Gather all data using Allgather
    std::vector<Complex> global_data(max_data_size * size);
    MPI_Allgather(padded_local.data(), max_data_size, MPI_DOUBLE_COMPLEX,
                  global_data.data(), max_data_size, MPI_DOUBLE_COMPLEX, comm);

    // Each process performs transpose on its portion
    int new_rows = cols / size;
    if (rank == size - 1) {
      new_rows = cols - (size - 1) * (cols / size);
    }
    int new_cols = rows;

    // Resize local_data for the new dimensions
    local_data.resize(new_rows * new_cols);
    std::fill(local_data.begin(), local_data.end(), Complex(0, 0));

    // Calculate my portion of the transpose
    int start_col = rank * (cols / size);
    int end_col = (rank == size - 1) ? cols : (rank + 1) * (cols / size);

    for (int p = 0; p < size; p++) {
      int p_rows = rows / size;
      if (p == size - 1) {
        p_rows = rows - (size - 1) * (rows / size);
      }

      for (int i = 0; i < p_rows; i++) {
        for (int j = start_col; j < end_col; j++) {
          int src_idx = p * max_data_size + i * cols + j;
          int local_j = j - start_col;
          if (src_idx < global_data.size()) {
            local_data[local_j * new_rows + i] = global_data[src_idx];
          }
        }
      }
    }

    // Update dimensions
    local_rows = new_rows;
    cols = new_cols;

    MPI_Barrier(comm);
    double trans_end = MPI_Wtime();
    if (rank == 0) {
      std::cout << "Sequential transpose time: " << trans_end - trans_start
                << " seconds" << std::endl;
    }
  }

  void collectResult(cv::Mat &magnitude_spectrum) {
    if (rank == 0) {
      magnitude_spectrum = cv::Mat(rows, cols, CV_64F);

      // Copy own portion
      for (int i = 0; i < local_rows; i++) {
        for (int j = 0; j < cols; j++) {
          magnitude_spectrum.at<double>(i, j) =
              20 * log(1 + std::abs(local_data[i * cols + j]));
        }
      }

      // Receive from other processes
      int base_rows = rows / size;
      for (int p = 1; p < size; p++) {
        int p_rows = (p == size - 1) ? rows - p * base_rows : base_rows;
        std::vector<Complex> temp_buffer(p_rows * cols);

        MPI_Status status;
        MPI_Recv(temp_buffer.data(), p_rows * cols, MPI_DOUBLE_COMPLEX, p, 0,
                 comm, &status);

        for (int i = 0; i < p_rows; i++) {
          for (int j = 0; j < cols; j++) {
            magnitude_spectrum.at<double>(p * base_rows + i, j) =
                20 * log(1 + std::abs(temp_buffer[i * cols + j]));
          }
        }
      }
    } else {
      // Send data to root
      MPI_Send(local_data.data(), local_rows * cols, MPI_DOUBLE_COMPLEX, 0, 0,
               comm);
    }

    // Convert to displayable range
    if (rank == 0) {
      cv::normalize(magnitude_spectrum, magnitude_spectrum, 0, 255,
                    cv::NORM_MINMAX);
      magnitude_spectrum.convertTo(magnitude_spectrum, CV_8U);
    }
  }
};

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);

  double total_start = MPI_Wtime();

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (argc != 2) {
    if (rank == 0) {
      std::cerr << "Usage: " << argv[0] << " <image_path>" << std::endl;
    }
    MPI_Abort(MPI_COMM_WORLD, 1);
    return -1;
  }

  cv::Mat image, combined_magnitude;
  std::vector<cv::Mat> channels, magnitude_spectrums;
  int rows = 0, cols = 0;

  if (rank == 0) {
    image = cv::imread(argv[1], cv::IMREAD_COLOR);
    if (image.empty()) {
      std::cerr << "Error: Could not read the image." << std::endl;
      MPI_Abort(MPI_COMM_WORLD, 1);
      return -1;
    }
    cv::split(image, channels);
    rows = image.rows;
    cols = image.cols;
  }

  // Broadcast dimensions
  MPI_Bcast(&rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&cols, 1, MPI_INT, 0, MPI_COMM_WORLD);

  // Process each channel
  for (int c = 0; c < 3; c++) {
    if (rank == 0) {
      std::cout << "\nProcessing channel " << c << std::endl;
    }

    PencilFFT fft(rows, cols);
    fft.distributeData(channels[c]);
    fft.computeFFT();

    cv::Mat magnitude_spectrum;
    fft.collectResult(magnitude_spectrum);

    if (rank == 0) {
      magnitude_spectrums.push_back(magnitude_spectrum);
    }
  }

  if (rank == 0) {
    cv::merge(magnitude_spectrums, combined_magnitude);
    const char *output_dir = std::getenv("PAR_OUTPUT_DIR");
    std::string output_path =
        std::string(output_dir) + "/parallel_fft_result.jpg";
    std::cout << "Saving output to " << output_path << std::endl;
    bool success = cv::imwrite(output_path, combined_magnitude);
    if (!success) {
      std::cout << "Failed to save output image" << std::endl;
    }
  }

  double total_end = MPI_Wtime();
  if (rank == 0) {
    std::cout << "\nTotal execution time: " << total_end - total_start
              << " seconds" << std::endl;
  }

  MPI_Finalize();
  return 0;
}
