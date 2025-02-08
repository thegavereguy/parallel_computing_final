#include <lib/shared.h>
#include <omp.h>

#include <utility>

void sequential(Conditions conditions, float* input, float* output) {
  output[0]                  = input[0];
  output[conditions.n_x - 1] = input[conditions.n_x - 1];

  for (int i = 0; i < conditions.n_t; i++) {
    for (int j = 1; j < conditions.n_x - 1; j++) {
      output[j] = input[j] + conditions.alpha *
                                 (input[j + 1] - 2 * input[j] + input[j - 1]);
    }
    std::swap(input, output);
  }
}

void parallel2_inner(Conditions conditions, float* input, float* output) {
  output[0]                  = input[0];
  output[conditions.n_x - 1] = input[conditions.n_x - 1];

  for (int i = 0; i < conditions.n_t; i++) {
#pragma omp parallel for num_threads(2)
    for (int j = 1; j < conditions.n_x - 1; j++) {
      output[j] = input[j] + conditions.alpha *
                                 (input[j + 1] - 2 * input[j] + input[j - 1]);
    }
    std::swap(input, output);
  }
}
void parallel4_inner(Conditions conditions, float* input, float* output) {
  output[0]                  = input[0];
  output[conditions.n_x - 1] = input[conditions.n_x - 1];

  for (int i = 0; i < conditions.n_t; i++) {
#pragma omp parallel for num_threads(4)
    for (int j = 1; j < conditions.n_x - 1; j++) {
      output[j] = input[j] + conditions.alpha *
                                 (input[j + 1] - 2 * input[j] + input[j - 1]);
    }
    std::swap(input, output);
  }
}
void parallel8_inner(Conditions conditions, float* input, float* output) {
  output[0]                  = input[0];
  output[conditions.n_x - 1] = input[conditions.n_x - 1];
  for (int i = 0; i < conditions.n_t; i++) {
#pragma omp parallel for num_threads(8)
    for (int j = 1; j < conditions.n_x - 1; j++) {
      output[j] = input[j] + conditions.alpha *
                                 (input[j + 1] - 2 * input[j] + input[j - 1]);
    }
    std::swap(input, output);
  }
}

void parallel2_outer(Conditions conditions, float* input, float* output) {
  output[0]                  = input[0];
  output[conditions.n_x - 1] = input[conditions.n_x - 1];

#pragma omp parallel for num_threads(2)
  for (int i = 0; i < conditions.n_t; i++) {
    for (int j = 1; j < conditions.n_x - 1; j++) {
      output[j] = input[j] + conditions.alpha *
                                 (input[j + 1] - 2 * input[j] + input[j - 1]);
    }
    std::swap(input, output);
  }
}
