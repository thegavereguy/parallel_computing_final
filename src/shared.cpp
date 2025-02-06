#include <lib/shared.h>

#include <utility>

void sequential(Conditions conditions, float* input, float* output) {
  for (int i = 0; i < conditions.n_t; i++) {
    for (int j = 1; j < conditions.n_x - 1; j++) {
      output[j] = input[j] + conditions.alpha *
                                 (input[j + 1] - 2 * input[j] + input[j - 1]);
    }
    std::swap(input, output);
  }
}
