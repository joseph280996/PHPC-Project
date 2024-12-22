export PROJECT_ROOT="/Users/joseph280996/Code/School/PHPC/Project"
export PAR_OUTPUT_DIR="$PROJECT_ROOT/output/strong"

echo "Start fft transform"
export OMP_NUM_THREADS=1
./parallel_omp /Users/joseph280996/Code/School/PHPC/Project/data/scaled_images/image_250x250.png
export OMP_NUM_THREADS=2
./parallel_omp /Users/joseph280996/Code/School/PHPC/Project/data/scaled_images/image_353x353.png
export OMP_NUM_THREADS=3
./parallel_omp /Users/joseph280996/Code/School/PHPC/Project/data/scaled_images/image_433x433.png
export OMP_NUM_THREADS=4
./parallel_omp /Users/joseph280996/Code/School/PHPC/Project/data/scaled_images/image_500x500.png
export OMP_NUM_THREADS=5
./parallel_omp /Users/joseph280996/Code/School/PHPC/Project/data/scaled_images/image_559x559.png
export OMP_NUM_THREADS=6
./parallel_omp /Users/joseph280996/Code/School/PHPC/Project/data/scaled_images/image_612x612.png
export OMP_NUM_THREADS=7
./parallel_omp /Users/joseph280996/Code/School/PHPC/Project/data/scaled_images/image_661x661.png
export OMP_NUM_THREADS=9
./parallel_omp /Users/joseph280996/Code/School/PHPC/Project/data/scaled_images/image_750x750.png
export OMP_NUM_THREADS=10
./parallel_omp /Users/joseph280996/Code/School/PHPC/Project/data/scaled_images/image_790x790.png
export OMP_NUM_THREADS=13
./parallel_omp /Users/joseph280996/Code/School/PHPC/Project/data/scaled_images/image_901x901.png
export OMP_NUM_THREADS=15
./parallel_omp /Users/joseph280996/Code/School/PHPC/Project/data/scaled_images/image_968x968.png
export OMP_NUM_THREADS=18
./parallel_omp /Users/joseph280996/Code/School/PHPC/Project/data/scaled_images/image_1060x1060.png
export OMP_NUM_THREADS=20
./parallel_omp /Users/joseph280996/Code/School/PHPC/Project/data/scaled_images/image_1118x1118.png
export OMP_NUM_THREADS=25
./parallel_omp /Users/joseph280996/Code/School/PHPC/Project/data/scaled_images/image_1250x1250.png
export OMP_NUM_THREADS=30
./parallel_omp /Users/joseph280996/Code/School/PHPC/Project/data/scaled_images/image_1369x1369.png
export OMP_NUM_THREADS=40
./parallel_omp /Users/joseph280996/Code/School/PHPC/Project/data/scaled_images/image_1581x1581.png
echo "Finished fft transform"
