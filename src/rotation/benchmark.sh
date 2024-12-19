#!/bin/bash
rm -rf build
mkdir build && cd build
cmake ..
make
echo "Start rotate image clockwise"
mpirun -np 8 ./parallel_rotate /home/johann/Desktop/PHPC-Project/data/input.jpg c
echo "Finished rotating image clockwise"
echo "Start rotate image counterclockwise"
mpirun -np 8 ./parallel_rotate /home/johann/Desktop/PHPC-Project/data/input.jpg cc
echo "Finished rotating image counterclockwise"
