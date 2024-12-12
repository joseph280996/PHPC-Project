#include <mpi.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <string>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (argc != 3) {
        if (rank == 0) {
            std::cerr << "Usage: " << argv[0] << " <input_image> <output_image>" << std::endl;
        }
        MPI_Finalize();
        return 1;
    }
    
    const char* input_filename = argv[1];
    const char* output_filename = argv[2];
    
    // Image dimensions and data
    int dims[3] = {0, 0, 0}; // height, width, channels
    std::vector<uchar> imageData;
    
    // Root process reads the image
    if (rank == 0) {
        cv::Mat img = cv::imread(input_filename, cv::IMREAD_UNCHANGED);
        if (img.empty()) {
            std::cerr << "Error: Could not read image " << input_filename << std::endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
            return 1;
        }
        
        // Store dimensions
        dims[0] = img.rows;
        dims[1] = img.cols;
        dims[2] = img.channels();
        
        // Ensure the image is continuous
        if (!img.isContinuous()) {
            img = img.clone();
        }
        
        // Convert image to vector
        imageData.assign(img.data, img.data + img.total() * img.channels());
    }
    
    // Broadcast dimensions to all processes
    MPI_Bcast(dims, 3, MPI_INT, 0, MPI_COMM_WORLD);
    
    int height = dims[0];
    int width = dims[1];
    int channels = dims[2];
    int rowSize = width * channels;
    
    // Calculate chunk sizes for each process
    int heightPerProcess = height / size;
    int myStartRow = rank * heightPerProcess;
    int myNumRows = (rank == size - 1) ? 
                    (height - myStartRow) : heightPerProcess;
    int myChunkSize = myNumRows * rowSize;
    
    // Allocate memory for local chunks
    std::vector<uchar> myChunk(myChunkSize);
    std::vector<uchar> flippedChunk(myChunkSize);
    
    // Distribute data using MPI_Scatterv for varying chunk sizes
    std::vector<int> sendcounts(size);
    std::vector<int> displs(size);
    
    if (rank == 0) {
        // Calculate send counts and displacements
        for (int i = 0; i < size; i++) {
            int startRow = i * heightPerProcess;
            int numRows = (i == size - 1) ? (height - startRow) : heightPerProcess;
            sendcounts[i] = numRows * rowSize;
            displs[i] = startRow * rowSize;
        }
    }
    
    // Scatter the image data
    MPI_Scatterv(rank == 0 ? imageData.data() : nullptr,
                 sendcounts.data(), displs.data(), MPI_BYTE,
                 myChunk.data(), myChunkSize, MPI_BYTE,
                 0, MPI_COMM_WORLD);
    
    // Flip the local chunk vertically
    for (int row = 0; row < myNumRows; row++) {
        std::copy(myChunk.data() + row * rowSize,
                 myChunk.data() + (row + 1) * rowSize,
                 flippedChunk.data() + (myNumRows - 1 - row) * rowSize);
    }
    
    // Gather all flipped chunks back to root
    std::vector<uchar> gatheredData;
    if (rank == 0) {
        gatheredData.resize(height * width * channels);
    }
    
    MPI_Gatherv(flippedChunk.data(), myChunkSize, MPI_BYTE,
                rank == 0 ? gatheredData.data() : nullptr,
                sendcounts.data(), displs.data(), MPI_BYTE,
                0, MPI_COMM_WORLD);
    
    // Root process saves the final image
    if (rank == 0) {
        cv::Mat outImage(height, width, CV_8UC(channels), gatheredData.data());
        cv::imwrite(output_filename, outImage);
    }
    
    MPI_Finalize();
    return 0;
}
