# CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(parallel_image_flip)

# Find packages
find_package(OpenCV REQUIRED)
find_package(MPI REQUIRED)

# Add executable
add_executable(parallel_flip main.cpp)

# Include directories
target_include_directories(parallel_flip PRIVATE ${OpenCV_INCLUDE_DIRS} ${MPI_INCLUDE_PATH})

# Link libraries
target_link_libraries(parallel_flip PRIVATE ${OpenCV_LIBS} ${MPI_CXX_LIBRARIES})

# Add MPI compile flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MPI_CXX_COMPILE_FLAGS}")
