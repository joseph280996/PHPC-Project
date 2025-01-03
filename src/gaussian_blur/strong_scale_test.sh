#!/bin/bash
export PROJECT_ROOT="/Users/joseph280996/Code/School/PHPC/Project"
export PAR_OUTPUT_DIR="$PROJECT_ROOT/output/strong"

echo "Start gaussian blur test"
export OMP_NUM_THREADS=1
./parallel_omp /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg
export OMP_NUM_THREADS=2
./parallel_omp /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg
export OMP_NUM_THREADS=3
./parallel_omp /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg
export OMP_NUM_THREADS=4
./parallel_omp /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg
export OMP_NUM_THREADS=5
./parallel_omp /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg
export OMP_NUM_THREADS=6
./parallel_omp /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg
export OMP_NUM_THREADS=7
./parallel_omp /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg
export OMP_NUM_THREADS=8
./parallel_omp /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg
export OMP_NUM_THREADS=9
./parallel_omp /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg
export OMP_NUM_THREADS=10
./parallel_omp /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg
export OMP_NUM_THREADS=11
./parallel_omp /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg
export OMP_NUM_THREADS=12
./parallel_omp /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg
export OMP_NUM_THREADS=13
./parallel_omp /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg
export OMP_NUM_THREADS=14
./parallel_omp /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg
export OMP_NUM_THREADS=15
./parallel_omp /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg
export OMP_NUM_THREADS=16
./parallel_omp /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg
export OMP_NUM_THREADS=17
./parallel_omp /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg
export OMP_NUM_THREADS=18
./parallel_omp /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg
export OMP_NUM_THREADS=19
./parallel_omp /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg
export OMP_NUM_THREADS=20
./parallel_omp /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg
export OMP_NUM_THREADS=25
./parallel_omp /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg
export OMP_NUM_THREADS=30
./parallel_omp /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg
export OMP_NUM_THREADS=35
./parallel_omp /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg
export OMP_NUM_THREADS=40
./parallel_omp /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg
export OMP_NUM_THREADS=50
./parallel_omp /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg
export OMP_NUM_THREADS=60
./parallel_omp /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg
echo "Finished gaussian blur test"
