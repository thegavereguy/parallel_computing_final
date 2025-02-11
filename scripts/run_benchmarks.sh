#!/bin/bash

cpu_benches=( "cpu_seq" "cpu_par2" "cpu_par4" "cpu_par8" );
gpu_benches=(  "gpugrid" );

if [ -z "$BENCHMARK_SAMPLES" ]; then
	export BENCHMARK_SAMPLES=5;
fi

if [ -z "$BENCHMARK_CONFIDENCE_INTERVAL" ]; then
	export BENCHMARK_CONFIDENCE_INTERVAL=0.30;
fi

echo "Running $BENCHMARK_SAMPLES samples per benchmark with $BENCHMARK_CONFIDENCE_INTERVAL confidence interval";
# go to folder /build/tests relative to the project root
cd $PROJECT_ROOT/build/tests

for i in ${cpu_benches[@]}; do
		echo "Running benchmark for [$i]";
		eval "./cpu_bench" \"[$i]\" "-r csv" "--benchmark-samples=$BENCHMARK_SAMPLES" "--benchmark-confidence-interval=$BENCHMARK_CONFIDENCE_INTERVAL" > "$PROJECT_ROOT/results/$i.csv";
done
for i in ${gpu_benches[@]}; do
		echo "Running benchmark for [$i] ";
		eval "./gpu_bench" \"[$i]\" "-r csv" "--benchmark-samples=$BENCHMARK_SAMPLES" "--benchmark-confidence-interval=$BENCHMARK_CONFIDENCE_INTERVAL" > "$PROJECT_ROOT/results/$i.csv";
done

