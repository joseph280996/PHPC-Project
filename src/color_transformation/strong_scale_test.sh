#!/bin/bash
export PROJECT_ROOT="/Users/joseph280996/Code/School/PHPC/Project"
export PAR_OUTPUT_DIR="$PROJECT_ROOT/output/strong"

echo "Start strong scalability test on image color transform"
mpirun -np 1 ./parallel_color_transformation /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg 23 255 52 false
mpirun -np 2 ./parallel_color_transformation /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg 23 255 52 false
mpirun -np 3 ./parallel_color_transformation /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg 23 255 52 false
mpirun -np 4 ./parallel_color_transformation /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg 23 255 52 false
mpirun -np 5 ./parallel_color_transformation /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg 23 255 52 false
mpirun -np 6 ./parallel_color_transformation /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg 23 255 52 false
mpirun -np 7 ./parallel_color_transformation /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg 23 255 52 false
mpirun -np 8 ./parallel_color_transformation /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg 23 255 52 false
mpirun -np 9 ./parallel_color_transformation /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg 23 255 52 false
mpirun -np 10 ./parallel_color_transformation /Users/joseph280996/Code/School/PHPC/Project/data/input.jpg 23 255 52 false
echo "Finished strong scalability on color transformation"

