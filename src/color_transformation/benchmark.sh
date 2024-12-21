#!/bin/bash
export PROJECT_ROOT="/Users/joseph280996/Code/School/PHPC/Project"
export SEQ_OUTPUT_DIR="$PROJECT_ROOT/output/sequential"
export PAR_OUTPUT_DIR="$PROJECT_ROOT/output/parallel"

echo "Start image color transform"
mpirun -np 8 ./parallel_color_transformation /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg 23 255 52
echo "Finished color transformation"

