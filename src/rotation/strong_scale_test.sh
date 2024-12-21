#!/bin/bash

export PROJECT_ROOT="/Users/joseph280996/Code/School/PHPC/Project"
export PAR_OUTPUT_DIR="$PROJECT_ROOT/output/strong"

echo "Start Strong Scalability Test Image Rotation"
mpirun -np 1 ./parallel_rotate /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg c false
mpirun -np 2 ./parallel_rotate /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg c false
mpirun -np 3 ./parallel_rotate /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg c false
mpirun -np 4 ./parallel_rotate /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg c false
mpirun -np 5 ./parallel_rotate /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg c false
mpirun -np 6 ./parallel_rotate /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg c false
mpirun -np 7 ./parallel_rotate /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg c false
mpirun -np 8 ./parallel_rotate /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg c false
mpirun -np 9 ./parallel_rotate /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg c false
mpirun -np 10 ./parallel_rotate /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg c false
echo "Finished Strong Scalability Test Image Rotation"
