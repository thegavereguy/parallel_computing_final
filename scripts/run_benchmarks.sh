#!/bin/bash

cpu_benches=( "cpu_seq" "cpu_seq_unr" "cpu_par2" "cpu_par4" "cpu_par8" "cpu_par4_all" );
gpu_benches=(  "gpu256" "gpu512" "gpu1024");

if [ -z "$BENCHMARK_SAMPLES" ]; then
	export BENCHMARK_SAMPLES=5;
fi

if [ -z "$BENCHMARK_CONFIDENCE_INTERVAL" ]; then
	export BENCHMARK_CONFIDENCE_INTERVAL=0.30;
fi

if [ -z "$BUILD_VULKAN" ]; then
	export BUILD_VULKAN=false
fi

echo "Running $BENCHMARK_SAMPLES samples per benchmark with $BENCHMARK_CONFIDENCE_INTERVAL confidence interval";
# go to folder /build/tests relative to the project root
cd $PROJECT_ROOT/build/tests

for i in ${cpu_benches[@]}; do
		echo "Running benchmark for [$i]";
		eval "./cpu_bench" \"[$i]\" "-r csv" "--benchmark-samples=$BENCHMARK_SAMPLES" "--benchmark-confidence-interval=$BENCHMARK_CONFIDENCE_INTERVAL" > "$PROJECT_ROOT/results/$i.csv";
done

if [ ${BUILD_VULKAN} = true ]; then
	for i in ${gpu_benches[@]}; do
		echo "Running benchmark for [$i] ";
		eval "./gpu_bench" \"[$i]\" "-r csv" "--benchmark-samples=$BENCHMARK_SAMPLES" "--benchmark-confidence-interval=$BENCHMARK_CONFIDENCE_INTERVAL" > "$PROJECT_ROOT/results/$i.csv";
	done
else
	echo "Skipping Vulkan benchmarks";
fi

if [ ${BUILD_MPI} = true ]; then
	echo "Running MPI benchmarks";

	# L=(1.0 1.0 1.0 1.0 1.25 1.25 1.25 1.5)
	# alpha=(0.001 0.0015 0.002 0.0025 0.003 0.0035 0.004 0.0045)
	# t_final=(1.0 1.25 1.5 1.75 2.0 2.25 2.25 2.5)
	# n_x=(256 512 1024 2048 4096 8192 16384 32768)
	# n_t=(2000 65000 130000 195000 260000 325000 390000 455000)

L=(1.0 1.0 1.0 1.0 1.25 1.25 1.25 1.25)
alpha=(0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001)
t_final=(1.0 1.0 1.5 1.5 2.0 2.0 2.5 2.5)
n_x=(1024 4096 16384 65536 262144 524288 1048576 2097152)
n_t=(100000 100000 150000 150000 200000 200000 250000 250000)

	n=( 2 4 8  )

	for iter in "${n[@]}"; do
		echo "Running MPI benchmark for $iter processes";
		echo "NX,NT,MEAN,MINT,MAXT,ITER" > "$PROJECT_ROOT/results/mpi_$iter.csv"
		for i in "${!L[@]}"; do
			../apps/mpi  ${L[i]}  ${alpha[i]}  ${t_final[i]} ${n_x[i]} ${n_t[i]} $iter >> "$PROJECT_ROOT/results/mpi_$iter.csv"
		done
	done
	else
	echo "Skipping MPI benchmarks";
fi
