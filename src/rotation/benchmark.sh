#!/bin/bash

export PROJECT_ROOT="/Users/joseph280996/Code/School/PHPC/Project"
export SEQ_OUTPUT_DIR="$PROJECT_ROOT/output/sequential"
export PAR_OUTPUT_DIR="$PROJECT_ROOT/output/parallel"

echo "Start rotate image clockwise"
mpirun -np 8 ./parallel_rotate /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg c
echo "Finished rotating image clockwise"
echo "Start rotate image counterclockwise"
mpirun -np 8 ./parallel_rotate /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg cc
echo "Finished rotating image counterclockwise"
