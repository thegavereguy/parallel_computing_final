#include <lib/shared.h>
#include <omp.h>

#include <cmath>
#include <cstdio>
#include <utility>

void sequential(Conditions conditions, float* input, float* output) {
  output[0]                  = input[0];
  output[conditions.n_x - 1] = input[conditions.n_x - 1];
  float dt                   = conditions.t_final / (conditions.n_t - 1);
  float dx                   = conditions.L / (conditions.n_x - 1);

  for (int i = 0; i < conditions.n_t; i++) {
    for (int j = 1; j < conditions.n_x - 1; j++) {
      output[j] =
          input[j] + conditions.alpha * (dt / (dx * dx)) *
                         (input[j + 1] - 2 * input[j] + input[j - 1]);  // d^2u
    }
    std::swap(input, output);
  }
}

void parallel2_inner(Conditions conditions, float* input, float* output) {
  output[0]                  = input[0];
  output[conditions.n_x - 1] = input[conditions.n_x - 1];
  float dt                   = conditions.t_final / (conditions.n_t - 1);
  float dx                   = conditions.L / (conditions.n_x - 1);

  for (int i = 0; i < conditions.n_t; i++) {
#pragma omp parallel for num_threads(2)
    for (int j = 1; j < conditions.n_x - 1; j++) {
      output[j] =
          input[j] + conditions.alpha * (dt / (dx * dx)) *
                         (input[j + 1] - 2 * input[j] + input[j - 1]);  // d^2u
    }
    std::swap(input, output);
  }
}
void parallel4_inner(Conditions conditions, float* input, float* output) {
  output[0]                  = input[0];
  output[conditions.n_x - 1] = input[conditions.n_x - 1];
  float dt                   = conditions.t_final / (conditions.n_t - 1);
  float dx                   = conditions.L / (conditions.n_x - 1);

  for (int i = 0; i < conditions.n_t; i++) {
#pragma omp parallel for num_threads(4)
    for (int j = 1; j < conditions.n_x - 1; j++) {
      output[j] =
          input[j] + conditions.alpha * (dt / (dx * dx)) *
                         (input[j + 1] - 2 * input[j] + input[j - 1]);  // d^2u
    }
    std::swap(input, output);
  }
}
void parallel8_inner(Conditions conditions, float* input, float* output) {
  output[0]                  = input[0];
  output[conditions.n_x - 1] = input[conditions.n_x - 1];
  float dt                   = conditions.t_final / (conditions.n_t - 1);
  float dx                   = conditions.L / (conditions.n_x - 1);

  for (int i = 0; i < conditions.n_t; i++) {
#pragma omp parallel for num_threads(8)
    for (int j = 1; j < conditions.n_x - 1; j++) {
      output[j] =
          input[j] + conditions.alpha * (dt / (dx * dx)) *
                         (input[j + 1] - 2 * input[j] + input[j - 1]);  // d^2u
    }
    std::swap(input, output);
  }
}

void parallel2_outer(Conditions conditions, float* input, float* output) {
  output[0]                  = input[0];
  output[conditions.n_x - 1] = input[conditions.n_x - 1];
  float dt                   = conditions.t_final / (conditions.n_t - 1);
  float dx                   = conditions.L / (conditions.n_x - 1);

#pragma omp parallel num_threads(2)
  for (int i = 0; i < conditions.n_t; i++) {
    for (int j = 1; j < conditions.n_x - 1; j++) {
      output[j] =
          input[j] + conditions.alpha * (dt / (dx * dx)) *
                         (input[j + 1] - 2 * input[j] + input[j - 1]);  // d^2u
    }
#pragma omp barrier
    std::swap(input, output);
  }
}

void sequential_unroll(Conditions conditions, float* input, float* output) {
  output[0]                  = input[0];
  output[conditions.n_x - 1] = input[conditions.n_x - 1];
  float dt                   = conditions.t_final / (conditions.n_t - 1);
  float dx                   = conditions.L / (conditions.n_x - 1);

  for (int i = 0; i < conditions.n_t; i++) {
    for (int j = 1; j < conditions.n_x - 1; j += 4) {
      output[j] =
          input[j] + conditions.alpha * (dt / (dx * dx)) *
                         (input[j + 1] - 2 * input[j] + input[j - 1]);  // d^2u
      output[j + 1] =
          input[j + 1] +
          conditions.alpha * (input[j + 2] - 2 * input[j + 1] + input[j]);
      output[j + 2] =
          input[j + 2] +
          conditions.alpha * (input[j + 3] - 2 * input[j + 2] + input[j + 1]);
      output[j + 3] =
          input[j + 3] +
          conditions.alpha * (input[j + 4] - 2 * input[j + 3] + input[j + 2]);
    }
    std::swap(input, output);
  }
}

void parallel2_collapse(Conditions conditions, float* input, float* output) {
  output[0]                  = input[0];
  output[conditions.n_x - 1] = input[conditions.n_x - 1];
  float dt                   = conditions.t_final / (conditions.n_t - 1);
  float dx                   = conditions.L / (conditions.n_x - 1);

#pragma omp parallel for num_threads(2) collapse(2)
  for (int i = 0; i < conditions.n_t; i++) {
    for (int j = 1; j < conditions.n_x - 1; j++) {
      output[j] =
          input[j] + conditions.alpha * (dt / (dx * dx)) *
                         (input[j + 1] - 2 * input[j] + input[j - 1]);  // d^2u
    }
  }
}

void prototype(Conditions conditions, float* input, float* output) {
  float dt = conditions.t_final / (conditions.n_t - 1);
  float dx = conditions.L / (conditions.n_x - 1);

  float* tmp_in = (float*)omp_aligned_alloc(32, conditions.n_x * sizeof(float));
  float* tmp_out =
      (float*)omp_aligned_alloc(32, conditions.n_x * sizeof(float));
  for (int i = 0; i < conditions.n_x; i++) {
    tmp_in[i]  = input[i];
    tmp_out[i] = 0;
  }

  tmp_out[0]                  = tmp_in[0];
  tmp_out[conditions.n_x - 1] = tmp_in[conditions.n_x - 1];
  // Set OpenMP thread affinity
  // omp_set_dynamic(0);
  omp_set_num_threads(omp_get_max_threads());

  for (int t = 0; t < conditions.n_t; t++) {
#pragma omp parallel
    {
#pragma omp for simd aligned(tmp_in, tmp_out : 32) schedule(static)
      for (int j = 1; j < conditions.n_x - 1; j += 1) {
        tmp_out[j] = tmp_in[j] + conditions.alpha * (dt / (dx * dx)) *
                                     (tmp_in[j + 1] - 2 * tmp_in[j] +
                                      tmp_in[j - 1]);  // d^2u
        // tmp_out[i + 1] = tmp_in[i + 1] +
        //                  conditions.alpha *
        //                      (tmp_in[i] - 2.0 * tmp_in[i + 1] + tmp_in[i +
        //                      2]);
        // tmp_out[i + 2] =
        //     tmp_in[i + 2] +
        //     conditions.alpha *
        //         (tmp_in[i + 1] - 2.0 * tmp_in[i + 2] + tmp_in[i + 3]);
        // tmp_out[i + 3] =
        //     tmp_in[i + 3] +
        //     conditions.alpha *
        //         (tmp_in[i + 2] - 2.0 * tmp_in[i + 3] + tmp_in[i + 4]);
      }
    }

    std::swap(tmp_in, tmp_out);
  }
  for (int i = 0; i < conditions.n_x; i++) {
    output[i] = tmp_in[i];
  }
  omp_free(tmp_out);
  omp_free(tmp_in);
}
