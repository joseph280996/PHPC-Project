export PROJECT_ROOT="/Users/joseph280996/Code/School/PHPC/Project"
export PAR_OUTPUT_DIR="$PROJECT_ROOT/output/strong"

echo "Start flipping vertical transform"
mpirun -np 1 ./parallel_flip /Users/joseph280996/Code/School/PHPC/Project/data/scaled_images/image_250x250.png v false
mpirun -np 2 ./parallel_flip /Users/joseph280996/Code/School/PHPC/Project/data/scaled_images/image_353x353.png v false
mpirun -np 3 ./parallel_flip /Users/joseph280996/Code/School/PHPC/Project/data/scaled_images/image_433x433.png v false
mpirun -np 4 ./parallel_flip /Users/joseph280996/Code/School/PHPC/Project/data/scaled_images/image_500x500.png v false
mpirun -np 5 ./parallel_flip /Users/joseph280996/Code/School/PHPC/Project/data/scaled_images/image_559x559.png v false
mpirun -np 6 ./parallel_flip /Users/joseph280996/Code/School/PHPC/Project/data/scaled_images/image_612x612.png v false
mpirun -np 7 ./parallel_flip /Users/joseph280996/Code/School/PHPC/Project/data/scaled_images/image_661x661.png v false
mpirun -np 9 ./parallel_flip /Users/joseph280996/Code/School/PHPC/Project/data/scaled_images/image_750x750.png v false
mpirun -np 10 ./parallel_flip /Users/joseph280996/Code/School/PHPC/Project/data/scaled_images/image_790x790.png v false
echo "Finished flipping vertical transform"
echo "Start flipping horizontal transform"
mpirun -np 1 ./parallel_flip /Users/joseph280996/Code/School/PHPC/Project/data/scaled_images/image_250x250.png h false
mpirun -np 2 ./parallel_flip /Users/joseph280996/Code/School/PHPC/Project/data/scaled_images/image_353x353.png h false
mpirun -np 3 ./parallel_flip /Users/joseph280996/Code/School/PHPC/Project/data/scaled_images/image_433x433.png h false
mpirun -np 4 ./parallel_flip /Users/joseph280996/Code/School/PHPC/Project/data/scaled_images/image_500x500.png h false
mpirun -np 5 ./parallel_flip /Users/joseph280996/Code/School/PHPC/Project/data/scaled_images/image_559x559.png h false
mpirun -np 6 ./parallel_flip /Users/joseph280996/Code/School/PHPC/Project/data/scaled_images/image_612x612.png h false
mpirun -np 7 ./parallel_flip /Users/joseph280996/Code/School/PHPC/Project/data/scaled_images/image_661x661.png h false
mpirun -np 9 ./parallel_flip /Users/joseph280996/Code/School/PHPC/Project/data/scaled_images/image_750x750.png h false
mpirun -np 10 ./parallel_flip /Users/joseph280996/Code/School/PHPC/Project/data/scaled_images/image_790x790.png h false
echo "Finished flipping horizontal transform"
