export BENCHMARK_CONFIDENCE_INTERVAL=0.30
export BENCH_SAMPLES=3
export BUILD_VULKAN=true
export BUILD_MPI=true
export BUILD_ACC=true
export DOWNLOAD_VULKAN_SDK=false
export CLEAR_RESULTS=0

export PROJECT_ROOT=$(pwd)

######################################################

mkdir results > /dev/null

if [ ${CLEAR_RESULTS} == 1 ]; then
	echo "Clearing previous results";
	rm -rf results/*
else
  echo "Keeping previous results";
fi

# Print information about the cpu
lscpu > results/cpu_info.txt
lspci > results/gpu_info.txt

# Creta a build directory if it does not exist
mkdir build > /dev/null
cd build

cmake ..

# Remove the previous build
# make clean

make all -j4
cd ..

sh $PROJECT_ROOT/scripts/run_benchmarks.sh
