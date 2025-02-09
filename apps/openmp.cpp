#include <fmt/base.h>
#include <fmt/core.h>
#include <lib/shared.h>

int main() {
  Conditions conditions = {1, 0.02, 1, 262144, 10000};

  float* input              = new float[conditions.n_x];
  input[0]                  = 100;
  input[conditions.n_x - 1] = 200;
  float* output             = new float[conditions.n_x];

  sequential(conditions, input, output);

  // print second, second to last and middle elements
  fmt::print("Second element: {}\n", output[1]);
  fmt::print("Second to last element: {}\n", output[conditions.n_x - 2]);
  fmt::print("Middle element: {}\n", output[conditions.n_x / 2]);

  delete[] input;
  delete[] output;

  input                     = new float[conditions.n_x];
  input[0]                  = 100;
  input[conditions.n_x - 1] = 200;
  output                    = new float[conditions.n_x];

  parallel2_inner(conditions, input, output);

  // print second, second to last and middle elements
  fmt::print("Second element: {}\n", output[1]);
  fmt::print("Second to last element: {}\n", output[conditions.n_x - 2]);
  fmt::print("Middle element: {}\n", output[conditions.n_x / 2]);

  delete[] input;
  delete[] output;

#pragma omp parallel num_threads(2)
  {
    fmt::print("helo");
  }

  return 0;
}
