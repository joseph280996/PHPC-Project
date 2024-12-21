#!/bin/bash
mkdir build 
cd build
cmake ..
make

mv parallel_openmp ..
mv sequential ..
