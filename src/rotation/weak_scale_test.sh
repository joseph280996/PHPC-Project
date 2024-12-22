export PROJECT_ROOT="/Users/joseph280996/Code/School/PHPC/Project"
export PAR_OUTPUT_DIR="$PROJECT_ROOT/output/strong"

echo "Start rotating vertical transform"
mpirun -np 1 ./parallel_rotate /Users/joseph280996/Code/School/PHPC/Project/data/scaled_images/image_250x250.png c false
mpirun -np 2 ./parallel_rotate /Users/joseph280996/Code/School/PHPC/Project/data/scaled_images/image_353x353.png c false
mpirun -np 3 ./parallel_rotate /Users/joseph280996/Code/School/PHPC/Project/data/scaled_images/image_433x433.png c false
mpirun -np 4 ./parallel_rotate /Users/joseph280996/Code/School/PHPC/Project/data/scaled_images/image_500x500.png c false
mpirun -np 5 ./parallel_rotate /Users/joseph280996/Code/School/PHPC/Project/data/scaled_images/image_559x559.png c false
mpirun -np 6 ./parallel_rotate /Users/joseph280996/Code/School/PHPC/Project/data/scaled_images/image_612x612.png c false
mpirun -np 7 ./parallel_rotate /Users/joseph280996/Code/School/PHPC/Project/data/scaled_images/image_661x661.png c false
mpirun -np 9 ./parallel_rotate /Users/joseph280996/Code/School/PHPC/Project/data/scaled_images/image_750x750.png c false
mpirun -np 10 ./parallel_rotate /Users/joseph280996/Code/School/PHPC/Project/data/scaled_images/image_790x790.png c false
echo "Finished rotating vertical transform"
