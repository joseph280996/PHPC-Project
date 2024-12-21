#!/bin/bash
export PROJECT_ROOT="/Users/joseph280996/Code/School/PHPC/Project"
export SEQ_OUTPUT_DIR="$PROJECT_ROOT/output/sequential"
export PAR_OUTPUT_DIR="$PROJECT_ROOT/output/parallel"

echo "Start fft transform"
# mpirun -np 8 ./parallel ~/Code/School/PHPC/Project/data/input.jpg > output.log 2>&1
export OMP_NUM_THREADS=10
./parallel_omp /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg
./sequential /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg
echo "Finished fft transform"
