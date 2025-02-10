#include <fmt/base.h>
#include <fmt/core.h>
#include <lib/shared.h>

#include <ctime>

int main() {
  // const double L       = 1.5;      // Length of the rod
  //  const double alpha   = 0.035;    // Thermal diffusivity
  //  const double t_final = 0.00025;  // Final time
  //  const int n_x        = 32768;    // Number of spatial points
  //  const int n_t        = 40000;
  Conditions conditions = {1.5, 0.035, 0.00025, 32768, 40000};

  fmt::print("nt {}", conditions.n_t);
  float* input              = new float[conditions.n_x];
  input[0]                  = 100;
  input[conditions.n_x - 1] = 200;
  float* output             = new float[conditions.n_x];
  struct timespec start, end;
  clock_gettime(CLOCK_MONOTONIC, &start);

  parallel4_inner(conditions, input, output);
  clock_gettime(CLOCK_MONOTONIC, &end);
  long seconds_ts              = end.tv_sec - start.tv_sec;
  long nanoseconds_ts          = end.tv_nsec - start.tv_nsec;
  double elapsed_clock_gettime = seconds_ts + nanoseconds_ts * 1e-9;

  fmt::print("Elapsed time using clock_gettime: {} seconds\n",
             elapsed_clock_gettime);

  // print second, second to last and middle elements
  fmt::print("Second element: {}\n", output[1]);
  fmt::print("Second to last element: {}\n", output[conditions.n_x - 2]);
  fmt::print("Middle element: {}\n", output[conditions.n_x / 2]);

  delete[] input;
  delete[] output;
  //
  //   input                     = new float[conditions.n_x];
  //   input[0]                  = 100;
  //   input[conditions.n_x - 1] = 200;
  //   output                    = new float[conditions.n_x];
  //
  //   parallel2_inner(conditions, input, output);
  //
  //   // print second, second to last and middle elements
  //   fmt::print("Second element: {}\n", output[1]);
  //   fmt::print("Second to last element: {}\n", output[conditions.n_x - 2]);
  //   fmt::print("Middle element: {}\n", output[conditions.n_x / 2]);
  //
  //   delete[] input;
  //   delete[] output;
  //
  // #pragma omp parallel num_threads(2)
  //   {
  //     fmt::print("helo");
  //   }

  return 0;
}
