#!/bin/bash

export PROJECT_ROOT="/Users/joseph280996/Code/School/PHPC/Project"
export PAR_OUTPUT_DIR="$PROJECT_ROOT/output/strong"

echo "Start Strong Scalability Test Image Vertical Flipping"
mpirun -np 1 ./parallel_flip /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg v false
mpirun -np 2 ./parallel_flip /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg v false
mpirun -np 3 ./parallel_flip /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg v false
mpirun -np 4 ./parallel_flip /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg v false
mpirun -np 5 ./parallel_flip /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg v false
mpirun -np 6 ./parallel_flip /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg v false
mpirun -np 7 ./parallel_flip /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg v false
mpirun -np 8 ./parallel_flip /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg v false
mpirun -np 9 ./parallel_flip /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg v false
mpirun -np 10 ./parallel_flip /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg v false
echo "Finished Strong Scalability Test Image Vertical Flipping"
echo "Start Strong Scalability Test Image Horizontal Flipping"
mpirun -np 1 ./parallel_flip /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg h false
mpirun -np 2 ./parallel_flip /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg h false
mpirun -np 3 ./parallel_flip /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg h false
mpirun -np 4 ./parallel_flip /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg h false
mpirun -np 5 ./parallel_flip /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg h false
mpirun -np 6 ./parallel_flip /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg h false
mpirun -np 7 ./parallel_flip /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg h false
mpirun -np 8 ./parallel_flip /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg h false
mpirun -np 9 ./parallel_flip /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg h false
mpirun -np 10 ./parallel_flip /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg h false
echo "Finished Strong Scalability Test Image Horizontal Flipping"
