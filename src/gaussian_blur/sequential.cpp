#include <chrono>
#include <cmath>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

class GaussianBlur {
private:
  // Generate 1D Gaussian kernel
  std::vector<float> createGaussianKernel(int radius, float sigma) {
    int size = 2 * radius + 1;
    std::vector<float> kernel(size);
    float sum = 0.0f;

    for (int x = -radius; x <= radius; x++) {
      float exponent = -(x * x) / (2.0f * sigma * sigma);
      kernel[x + radius] =
          std::exp(exponent) / (std::sqrt(2.0f * M_PI) * sigma);
      sum += kernel[x + radius];
    }

    // Normalize kernel
    for (int i = 0; i < size; i++) {
      kernel[i] /= sum;
    }

    return kernel;
  }

public:
  // Apply Gaussian blur to image data
  double applyBlur(std::vector<unsigned char> &image, int width, int height,
                   int channels, int radius, float sigma) {
    if (image.empty() || width <= 0 || height <= 0 || channels <= 0) {
      std::cerr << "Invalid image parameters" << std::endl;
      return 0.0;
    }

    auto start = std::chrono::high_resolution_clock::now();

    std::vector<unsigned char> temp(image.size());
    std::vector<float> kernel = createGaussianKernel(radius, sigma);

    // Horizontal pass
    for (int y = 0; y < height; y++) {
      for (int x = 0; x < width; x++) {
        for (int c = 0; c < channels; c++) {
          float sum = 0.0f;

          for (int i = -radius; i <= radius; i++) {
            int srcX = std::min(std::max(x + i, 0), width - 1);
            sum +=
                image[(y * width + srcX) * channels + c] * kernel[i + radius];
          }

          temp[(y * width + x) * channels + c] =
              static_cast<unsigned char>(std::min(std::max(sum, 0.0f), 255.0f));
        }
      }
    }

    // Vertical pass
    for (int y = 0; y < height; y++) {
      for (int x = 0; x < width; x++) {
        for (int c = 0; c < channels; c++) {
          float sum = 0.0f;

          for (int i = -radius; i <= radius; i++) {
            int srcY = std::min(std::max(y + i, 0), height - 1);
            sum += temp[(srcY * width + x) * channels + c] * kernel[i + radius];
          }

          image[(y * width + x) * channels + c] =
              static_cast<unsigned char>(std::min(std::max(sum, 0.0f), 255.0f));
        }
      }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    return duration.count();
  }
};

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <image_path>" << std::endl;
    return -1;
  }

  // Record total execution time
  auto total_start = std::chrono::high_resolution_clock::now();

  // Read image using OpenCV
  auto read_start = std::chrono::high_resolution_clock::now();
  cv::Mat image = cv::imread(argv[1]);
  if (image.empty()) {
    std::cerr << "Error: Could not read image " << argv[1] << std::endl;
    return -1;
  }
  auto read_end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> read_duration = read_end - read_start;

  // Convert OpenCV Mat to vector for our implementation
  std::vector<unsigned char> imageData(
      image.data, image.data + image.total() * image.channels());

  // Apply Gaussian blur and get processing time
  GaussianBlur gaussianBlur;
  double blur_time = gaussianBlur.applyBlur(imageData, image.cols, image.rows,
                                            image.channels(), 5, 2.0f);

  // Copy processed data back to Mat
  std::memcpy(image.data, imageData.data(), imageData.size());

  // Save the result using OpenCV
  auto write_start = std::chrono::high_resolution_clock::now();
  const char *output_dir = std::getenv("SEQ_OUTPUT_DIR");
  std::string outputPath =
      std::string(output_dir) + "/sequential_blurred_result.jpg";
  cv::imwrite(outputPath, image);
  auto write_end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> write_duration = write_end - write_start;

  auto total_end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> total_duration = total_end - total_start;

  // Print timing information
  std::cout << "\nTiming Information:" << std::endl;
  std::cout << "Blur Processing Time: " << blur_time << " seconds" << std::endl;
  std::cout << "Total Execution Time: " << total_duration.count() << " seconds"
            << std::endl;

  return 0;
}
