cmake_minimum_required(VERSION 3.10)
project(ParallelImageProcessing)

# Specify C++ standard - we need at least C++11 for OpenCV
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)
find_package(MPI REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS} ${MPI_INCLUDE_PATH} include)

add_executable(main parallel_io.cpp)
target_link_libraries(main ${OpenCV_LIBS} ${MPI_LIBRARIES})
