#!/bin/bash
export PROJECT_ROOT="/Users/joseph280996/Code/School/PHPC/Project"
export SEQ_OUTPUT_DIR="$PROJECT_ROOT/output/sequential"
export PAR_OUTPUT_DIR="$PROJECT_ROOT/output/parallel"

echo "Start flipping image horizontally"
mpirun -np 10 ./parallel_flip /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg h
echo "Finished flipping image horizontally"
echo "Start flipping image verticall"
mpirun -np 10 ./parallel_flip /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg v
echo "Finished flipping image vertically"
