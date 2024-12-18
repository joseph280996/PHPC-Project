#!/bin/bash
mkdir build && cd build
cmake ..
make
echo "Start flipping image horizontally"
mpirun -np 10 ./parallel_flip data/input.jpg h
echo "Finished flipping image horizontally"
echo "Start flipping image verticall"
mpirun -np 10 ./parallel_flip data/input.jpg v
echo "Finished flipping image vertically"
