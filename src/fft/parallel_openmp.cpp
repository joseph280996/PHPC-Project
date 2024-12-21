#include <cmath>
#include <complex>
#include <opencv2/opencv.hpp>
#include <vector>
#include <omp.h>

const double PI = 3.14159265358979323846;

using namespace std;
using Complex = complex<double>;

bool isPowerOf2(int n) { return n && !(n & (n - 1)); }

int nextPowerOf2(int n) {
  int power = 1;
  while (power < n) {
    power *= 2;
  }
  return power;
}

int bitReverse(int n, int bits) {
  int reversed = 0;
  for (int i = 0; i < bits; i++) {
    if (n & (1 << i)) {
      reversed |= 1 << (bits - 1 - i);
    }
  }
  return reversed;
}

// Forward-only FFT implementation
vector<Complex> fft(vector<Complex> &x) {
  int n = x.size();
  if (!isPowerOf2(n)) {
    throw runtime_error("Size must be a power of 2");
  }

  int bits = log2(n);
  vector<Complex> result(n);

  // Bit reversal
  for (int i = 0; i < n; i++) {
    result[bitReverse(i, bits)] = x[i];
  }

  // Butterfly operations
  for (int stage = 1; stage <= bits; stage++) {
    int m = 1 << stage;
    int half_m = m / 2;
    Complex wm = polar(1.0, -2 * PI / m);

    #pragma omp parallel for schedule(static)
    for (int k = 0; k < n; k += m) {
      Complex w = 1;
      for (int j = 0; j < half_m; j++) {
        Complex t = w * result[k + j + half_m];
        Complex u = result[k + j];
        result[k + j] = u + t;
        result[k + j + half_m] = u - t;
        w *= wm;
      }
    }
  }

  return result;
}

// Forward-only 2D FFT implementation
vector<vector<Complex>> fft2D(const cv::Mat &channel) {
  int rows = channel.rows;
  int cols = channel.cols;

  int padded_rows = nextPowerOf2(rows);
  int padded_cols = nextPowerOf2(cols);

  vector<vector<Complex>> complex_image(padded_rows,
                                      vector<Complex>(padded_cols));

  // Convert channel to complex numbers and pad
  #pragma omp parallel for collapse(2)
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      complex_image[i][j] = Complex(channel.at<uchar>(i, j), 0);
    }
  }

  // Apply FFT to rows
  #pragma omp parallel for schedule(dynamic)
  for (int i = 0; i < padded_rows; i++) {
    vector<Complex> row = complex_image[i];
    row = fft(row);
    complex_image[i] = row;
  }

  // Apply FFT to columns
  #pragma omp parallel for schedule(dynamic)
  for (int j = 0; j < padded_cols; j++) {
    vector<Complex> col(padded_rows);
    for (int i = 0; i < padded_rows; i++) {
      col[i] = complex_image[i][j];
    }
    col = fft(col);
    for (int i = 0; i < padded_rows; i++) {
      complex_image[i][j] = col[i];
    }
  }

  return complex_image;
}

cv::Mat getMagnitudeImage(const vector<vector<Complex>> &complex_image) {
  int rows = complex_image.size();
  int cols = complex_image[0].size();
  cv::Mat magnitude(rows, cols, CV_64F);

  double max_magnitude = 0;
  
  // Calculate magnitude and find maximum
  #pragma omp parallel
  {
    double local_max = 0;
    #pragma omp for collapse(2)
    for (int i = 0; i < rows; i++) {
      for (int j = 0; j < cols; j++) {
        magnitude.at<double>(i, j) = abs(complex_image[i][j]);
        local_max = max(local_max, magnitude.at<double>(i, j));
      }
    }
    #pragma omp critical
    {
      max_magnitude = max(max_magnitude, local_max);
    }
  }

  // Normalize and convert to logarithmic scale
  #pragma omp parallel for collapse(2)
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      magnitude.at<double>(i, j) = log(1 + magnitude.at<double>(i, j));
    }
  }

  // Normalize to 0-255 range
  cv::normalize(magnitude, magnitude, 0, 255, cv::NORM_MINMAX);

  // Convert to 8-bit image
  cv::Mat magnitude_8u;
  magnitude.convertTo(magnitude_8u, CV_8U);

  return magnitude_8u;
}

void shiftQuadrants(cv::Mat &magnitude_spectrum) {
  int cx = magnitude_spectrum.cols / 2;
  int cy = magnitude_spectrum.rows / 2;

  cv::Mat q0(magnitude_spectrum, cv::Rect(0, 0, cx, cy));
  cv::Mat q1(magnitude_spectrum, cv::Rect(cx, 0, cx, cy));
  cv::Mat q2(magnitude_spectrum, cv::Rect(0, cy, cx, cy));
  cv::Mat q3(magnitude_spectrum, cv::Rect(cx, cy, cx, cy));

  cv::Mat tmp;
  q0.copyTo(tmp);
  q3.copyTo(q0);
  tmp.copyTo(q3);

  q1.copyTo(tmp);
  q2.copyTo(q1);
  tmp.copyTo(q2);
}

int main(int argc, char **argv) {
  // Set number of OpenMP threads
  omp_set_num_threads(omp_get_max_threads());
  
  // Read RGB image
  cv::Mat image = cv::imread(argv[1], cv::IMREAD_COLOR);
  if (image.empty()) {
    cerr << "Error: Could not read the image." << endl;
    return -1;
  }

  // Split the image into channels
  vector<cv::Mat> channels;
  cv::split(image, channels);

  double start = omp_get_wtime();

  // Process each channel
  vector<cv::Mat> magnitude_spectrums(channels.size());
  #pragma omp parallel for schedule(dynamic)
  for (size_t i = 0; i < channels.size(); i++) {
    // Perform forward FFT
    auto complex_image = fft2D(channels[i]);

    // Get magnitude spectrum
    cv::Mat magnitude_spectrum = getMagnitudeImage(complex_image);

    // Shift zero frequency to center
    shiftQuadrants(magnitude_spectrum);

    magnitude_spectrums[i] = magnitude_spectrum;
  }

  double stop = omp_get_wtime();
  cout << "Parallel time with " << omp_get_max_threads() << " threads: "
       << (stop - start) * 1000 << " milliseconds"
       << endl;

  // Create combined RGB magnitude spectrum
  cv::Mat combined_magnitude;
  cv::merge(magnitude_spectrums, combined_magnitude);

  const char *output_dir = std::getenv("PAR_OUTPUT_DIR");
  // Save results
  string output_path = string(output_dir) + "/parallel_fft_result.jpg";
  cout << "Saving output to " << output_path << endl;
  bool success = cv::imwrite(output_path, combined_magnitude);
  if (!success) {
    std::cout << "Failed to save output image" << std::endl;
  }

  return 0;
}
