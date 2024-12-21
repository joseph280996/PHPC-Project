cmake_minimum_required(VERSION 3.10)
project(FFT)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenMP package
find_package(OpenMP REQUIRED)
if(OpenMP_CXX_FOUND)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

# Find OpenCV package
find_package(OpenCV REQUIRED)
if(OpenCV_FOUND)
  message(STATUS "OpenCV library status:")
  message(STATUS "    version: ${OpenCV_VERSION}")
  message(STATUS "    libraries: ${OpenCV_LIBS}")
  message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")
else()
  message(FATAL_ERROR "OpenCV not found. Please install OpenCV first.")
endif()

# Add executable
add_executable(parallel_openmp parallel_openmp.cpp)
add_executable(sequential sequential.cpp)

# Link OpenCV libraries
target_link_libraries(parallel_openmp
    PRIVATE
    ${OpenCV_LIBS}
    OpenMP::OpenMP_CXX
)

target_link_libraries(sequential
    PRIVATE
    ${OpenCV_LIBS}
    OpenMP::OpenMP_CXX
)

# Include directories
target_include_directories(parallel_openmp
    PRIVATE
    ${OpenCV_INCLUDE_DIRS}
)
target_include_directories(sequential
    PRIVATE
    ${OpenCV_INCLUDE_DIRS}
)

# Set compiler flags
if(MSVC)
  target_compile_options(parallel_openmp PRIVATE /W4)
  target_compile_options(sequential PRIVATE /W4)
else()
  target_compile_options(parallel_openmp PRIVATE -Wall -Wextra -Wpedantic)
  target_compile_options(sequential PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Optional: Enable optimization for Release builds
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
