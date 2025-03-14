# Parallel Image Manipulation

## Prerequisite

Make sure the followings in your system are installed on your system:

- OpenCV:
    - Windows: `scoop install main/opencv`
    - MacOS: `brew install opencv`
- CMake
- C++ compiler (either llvm with `brew install llvm` or gcc with `brew install
gcc`)
- Python 3.10+ (this is mainly for plotting)
    - Matplotlib
    - Numpy
    - Pillow

## Folder structure

```bash
📁 Project/                         # Root Directory
├── 📁 documents/                       # Folder holds all the documents in this project along with a LaTeX file for high-level pseudocode algorithm format
├── 📁 data/                            # Folder holds images for tests
│   ├── 📁 scaled_images/                   # Images resized for weak scalability tests
│   └── 📄 input.jpg                        # Base Image for resized, strong and comparison tests
├── 📁 src/                             # All the codes folder
│   ├── 📁 color_transformation/            # Color transform implementation
│   │   ├── 📄 benchmark.sh                     # Bash script that handle running the comparison tests
│   │   ├── 📄 build.sh                         # Bash scrip that build the program
│   │   ├── 📄 CMakeLists.txt                   # cmake config file
│   │   ├── 📄 main.cpp                         # C++ code for both sequential and parallel using OpenMPI
│   │   ├── 📄 strong_scale_test.sh             # Bash script that run strong scalability test
│   │   └── 📄 weak_scale_test.sh               # Bash script that run weak scalability test
│   ├── 📁 fft/                             # Fourier Transform implementation
│   │   ├── 📄 benchmark.sh                     # Bash Script that run comparison tests
│   │   ├── 📄 build.sh                         # Bash script that build the program
│   │   ├── 📄 CMakeLists.txt                   # cmake config
│   │   ├── 📄 parallel_openmp.cpp              # Parallel implementation using OpenMP
│   │   ├── 📄 sequential.cpp                   # Sequential implementation
│   │   ├── 📄 strong_scale_test.sh             # Bash script that run strong scalability tests
│   │   └── 📄 weak_scale_test.sh               # Bash script that run weak scalability tests
│   ├── 📁 flipping/                        # Flipping Transform implementation
│   │   ├── 📄 benchmark.sh                     # Bash Script that run comparison tests
│   │   ├── 📄 build.sh                         # Bash Script that build the program
│   │   ├── 📄 CMakeLists.txt                   # cmake config
│   │   ├── 📄 main.cpp                         # C++ code for both sequential and parallel with OpenMPI
│   │   ├── 📄 strong_scale_test.sh             # Bash script that run strong scalability tests
│   │   └── 📄 weak_scale_test.sh               # Bash script that run weak scalability tests
│   ├── 📁 gaussian_blur/                   # Gaussian Blur Transform Implementation
│   │   ├── 📄 benchmark.sh                     # Bash script that run comparison tests
│   │   ├── 📄 build.sh                         # Bash script that build the program
│   │   ├── 📄 CMakeLists.txt                   # cmake config
│   │   ├── 📄 parallel_omp.cpp                 # Parallel implementation using OpenMP
│   │   ├── 📄 sequential.cpp                   # Sequential implementation
│   │   ├── 📄 strong_scale_test.sh             # Bash script that run strong scalability tests
│   │   └── 📄 weak_scale_test.sh               # Bash script that run weak scalability tests
│   ├── 📁 rotation/                        # Rotation Implementation
│   │   ├── 📄 benchmark.sh                     # Bash script that run comparison tests
│   │   ├── 📄 build.sh                         # Bash script that build the program
│   │   ├── 📄 CMakeLists.txt                   # cmake config
│   │   ├── 📄 main.cpp                         # C++ code for both sequential and parallel with OpenMPI
│   │   ├── 📄 strong_scale_test.sh             # Bash script that run strong scalability tests
│   │   └── 📄 weak_scale_test.sh               # Bash script that run weak scalability tests
│   │   └── 📄 weak_scale_test.sh               # Bash script that run weak scalability tests
│   └── 📄 main.ipynb                       # Jupyter Notebook that handles all plotting
└── 📄 README.md                        # This README
```

## Instruction
For each transformation, we've provided a `build.sh`, `benchmark.sh`,
`strong_scale_test.sh`, and `weak_scale_test.sh` file that is described in the
previous section. Before running any tests, the program needs to be build
to generate necessary executables:

```bash
chmod +x build.sh
./build.sh
```

Then run the tests in question using:
```bash
chmod +x <test bash script>.sh
./<test bash script>.sh

```

To generate the plot, the code can be executed in VSCode by opening `main.ipynb`.
