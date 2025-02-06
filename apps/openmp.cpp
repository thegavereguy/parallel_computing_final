#include <fmt/core.h>
#include <lib/shared.h>

int main() {
  Conditions conditions = {1, 0.000002, 0.1, 262144, 100000};

  float* input  = new float[conditions.n_x];
  float* output = new float[conditions.n_x];

  sequential(conditions, input, output);

  // print second, second to last and middle elements
  fmt::print("Second element: {}\n", output[1]);
  fmt::print("Second to last element: {}\n", output[conditions.n_x - 2]);
  fmt::print("Middle element: {}\n", output[conditions.n_x / 2]);
  return 0;
}
