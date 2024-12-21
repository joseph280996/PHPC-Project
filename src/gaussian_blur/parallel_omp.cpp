#include <cmath>
#include <iostream>
#include <omp.h>
#include <opencv2/opencv.hpp>
#include <vector>

class GaussianBlur {
private:
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

    for (int i = 0; i < size; i++) {
      kernel[i] /= sum;
    }

    return kernel;
  }

public:
  void applyBlur(std::vector<unsigned char> &image, int width, int height,
                 int channels, int radius, float sigma) {
    std::vector<unsigned char> temp(image.size());
    std::vector<float> kernel = createGaussianKernel(radius, sigma);

#pragma omp parallel for collapse(2)
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

#pragma omp parallel for collapse(2)
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
  }
};

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <image_path>" << std::endl;
    return -1;
  }

  cv::Mat image = cv::imread(argv[1]);
  if (image.empty()) {
    std::cerr << "Error: Could not read image " << argv[1] << std::endl;
    return -1;
  }

  std::vector<unsigned char> imageData(
      image.data, image.data + image.total() * image.channels());

  GaussianBlur gaussianBlur;
  double start = omp_get_wtime();
  gaussianBlur.applyBlur(imageData, image.cols, image.rows, image.channels(), 5,
                         2.0f);
  double end = omp_get_wtime();

  std::memcpy(image.data, imageData.data(), imageData.size());
  const char *output_dir = std::getenv("PAR_OUTPUT_DIR");
  std::string outputPath =
      std::string(output_dir) + "/parallel_blurred_result.jpg";
  cv::imwrite(outputPath, image);

  std::cout << end - start << std::endl;

  return 0;
}
