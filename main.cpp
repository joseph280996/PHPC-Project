#include <opencv2/opencv.hpp>
#include <vector>
#include <iostream>
#include <string>

class ImageReader {
public:
    struct ImageData {
        std::vector<std::vector<std::vector<uint8_t>>> channels; // [height][width][channel]
        int width;
        int height;
    };

    static ImageData readImage(const std::string& filepath) {
        ImageData result;
        
        // Read the image using OpenCV
        cv::Mat img = cv::imread(filepath);
        if (img.empty()) {
            std::cerr << "Error: Could not read image: " << filepath << std::endl;
            return result;
        }

        // Convert BGR to RGB (OpenCV uses BGR by default)
        cv::cvtColor(img, img, cv::COLOR_BGR2RGB);

        // Get image dimensions
        result.height = img.rows;
        result.width = img.cols;

        // Initialize the 3D vector
        result.channels.resize(result.height, 
            std::vector<std::vector<uint8_t>>(result.width, 
                std::vector<uint8_t>(3)));

        // Copy data into our vector structure
        for (int y = 0; y < result.height; ++y) {
            for (int x = 0; x < result.width; ++x) {
                cv::Vec3b pixel = img.at<cv::Vec3b>(y, x);
                result.channels[y][x][0] = pixel[0]; // R
                result.channels[y][x][1] = pixel[1]; // G
                result.channels[y][x][2] = pixel[2]; // B
            }
        }

        return result;
    }

    // Helper function to save the image
    static bool saveImage(const std::string& filepath, const ImageData& data) {
        // Create OpenCV Mat from our data
        cv::Mat img(data.height, data.width, CV_8UC3);
        
        // Copy data back to Mat
        for (int y = 0; y < data.height; ++y) {
            for (int x = 0; x < data.width; ++x) {
                img.at<cv::Vec3b>(y, x)[0] = data.channels[y][x][0]; // R
                img.at<cv::Vec3b>(y, x)[1] = data.channels[y][x][1]; // G
                img.at<cv::Vec3b>(y, x)[2] = data.channels[y][x][2]; // B
            }
        }
        
        // Convert RGB to BGR for saving
        cv::cvtColor(img, img, cv::COLOR_RGB2BGR);
        
        return cv::imwrite(filepath, img);
    }

    // Helper function to display the image
    static void displayImage(const ImageData& data, const std::string& windowName = "Image") {
        cv::Mat img(data.height, data.width, CV_8UC3);
        
        for (int y = 0; y < data.height; ++y) {
            for (int x = 0; x < data.width; ++x) {
                img.at<cv::Vec3b>(y, x)[0] = data.channels[y][x][0]; // R
                img.at<cv::Vec3b>(y, x)[1] = data.channels[y][x][1]; // G
                img.at<cv::Vec3b>(y, x)[2] = data.channels[y][x][2]; // B
            }
        }
        
        // Convert RGB to BGR for display
        cv::cvtColor(img, img, cv::COLOR_RGB2BGR);
        
        cv::imshow(windowName, img);
        cv::waitKey(0);
        cv::destroyWindow(windowName);
    }
};

int main() {
    // Example usage
    std::string inputPath = "input.jpg";  // Change this to your image path
    std::string outputPath = "output.jpg";
    
    try {
        // Read image
        auto imageData = ImageReader::readImage(inputPath);
        
        if (imageData.width == 0 || imageData.height == 0) {
            std::cerr << "Failed to load image" << std::endl;
            return 1;
        }
        
        // Print image information
        std::cout << "Image loaded successfully:" << std::endl;
        std::cout << "Width: " << imageData.width << std::endl;
        std::cout << "Height: " << imageData.height << std::endl;
        
        // Example: Print RGB values of pixel at (0,0)
        std::cout << "\nPixel at (0,0):" << std::endl;
        std::cout << "R: " << static_cast<int>(imageData.channels[0][0][0]) << std::endl;
        std::cout << "G: " << static_cast<int>(imageData.channels[0][0][1]) << std::endl;
        std::cout << "B: " << static_cast<int>(imageData.channels[0][0][2]) << std::endl;
        
        // Display the image
        ImageReader::displayImage(imageData, "Loaded Image");
        
        // Save the image
        if (!ImageReader::saveImage(outputPath, imageData)) {
            std::cerr << "Failed to save image" << std::endl;
            return 1;
        }
        
        std::cout << "\nImage saved successfully to: " << outputPath << std::endl;
        
    } catch (const cv::Exception& e) {
        std::cerr << "OpenCV error: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
