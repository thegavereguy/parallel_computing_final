#include <fmt/base.h>
#include <lib/shared.h>

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/benchmark/catch_chronometer.hpp>
#include <catch2/benchmark/catch_clock.hpp>
#include <catch2/catch_get_random_seed.hpp>
#include <catch2/catch_test_case_info.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/reporters/catch_reporter_registrars.hpp>
#include <catch2/reporters/catch_reporter_streaming_base.hpp>

TEST_CASE("Sequential solution - Test", "[seq]") {
  Conditions conditions = {1, 0.5, 0.1, 16, 30};
  float* input          = new float[conditions.n_x];
  float* output         = new float[conditions.n_x];
  initialize_array(input, conditions.n_x);
  initialize_array(output, conditions.n_x);

  input[0]                  = 100;
  input[conditions.n_x - 1] = 200;

  sequential(conditions, input, output);
  for (int i = 0; i < conditions.n_x; i++) {
    REQUIRE_THAT(output[i], Catch::Matchers::WithinAbs(expected[i], 0.001));
  }
  delete[] input;
  delete[] output;
}
TEST_CASE("Parallel2 inner solution - Test", "[par2]") {
  Conditions conditions = {1, 0.5, 0.1, 16, 30};
  float* input          = new float[conditions.n_x];
  float* output         = new float[conditions.n_x];
  initialize_array(input, conditions.n_x);
  initialize_array(output, conditions.n_x);

  input[0]                  = 100;
  input[conditions.n_x - 1] = 200;

  for (int i = 0; i < conditions.n_x; i++) {
  }

  parallel2_inner(conditions, input, output);
  for (int i = 0; i < conditions.n_x; i++) {
    REQUIRE_THAT(output[i], Catch::Matchers::WithinAbs(expected[i], 0.001));
  }
  delete[] input;
  delete[] output;
}
TEST_CASE("Parallel4 inner solution - Test", "[par4]") {
  Conditions conditions = {1, 0.5, 0.1, 16, 30};
  float* input          = new float[conditions.n_x];
  float* output         = new float[conditions.n_x];
  initialize_array(input, conditions.n_x);
  initialize_array(output, conditions.n_x);
  input[0]                  = 100;
  input[conditions.n_x - 1] = 200;

  fmt::print("par4 solution\n");
  parallel4_inner(conditions, input, output);
  for (int i = 0; i < conditions.n_x; i++) {
    REQUIRE_THAT(output[i], Catch::Matchers::WithinAbs(expected[i], 0.001));
  }

  delete[] input;
  delete[] output;
}

TEST_CASE("Parallel8 inner solution - Test", "[par8]") {
  Conditions conditions = {1, 0.5, 0.1, 16, 30};
  float* input          = new float[conditions.n_x];
  float* output         = new float[conditions.n_x];
  initialize_array(input, conditions.n_x);
  initialize_array(output, conditions.n_x);
  input[0]                  = 100;
  input[conditions.n_x - 1] = 200;

  parallel8_inner(conditions, input, output);
  for (int i = 0; i < conditions.n_x; i++) {
    REQUIRE_THAT(output[i], Catch::Matchers::WithinAbs(expected[i], 0.001));
  }
  delete[] input;
  delete[] output;
}

// TEST_CASE("Parallel2 outer solution - Test", "[par2_out]") {
//   Conditions conditions = {1, 0.5, 0.1, 16, 30};
//   float* input          = new float[conditions.n_x];
//   float* output         = new float[conditions.n_x];
//   initialize_array(input, conditions.n_x);
//   initialize_array(output, conditions.n_x);
//   input[0]                  = 100;
//   input[conditions.n_x - 1] = 200;
//
//   parallel2_outer(conditions, input, output);
//   for (int i = 0; i < conditions.n_x; i++) {
//     REQUIRE_THAT(output[i], Catch::Matchers::WithinAbs(expected[i], 0.001));
//   }
//   delete[] input;
//   delete[] output;
// }
//
// TEST_CASE("Sequential unrolled solution - Test", "[seq_unr]") {
//   Conditions conditions = {1, 0.5, 0.1, 16, 30};
//   float* input          = new float[conditions.n_x];
//   float* output         = new float[conditions.n_x];
//   initialize_array(input, conditions.n_x);
//   initialize_array(output, conditions.n_x);
//   input[0]                  = 100;
//   input[conditions.n_x - 1] = 200;
//   sequential_unroll(conditions, input, output);
//   for (int i = 0; i < conditions.n_x; i++) {
//     fmt::print("{} ", output[i]);
//     // REQUIRE_THAT(output[i], Catch::Matchers::WithinAbs(expected[i],
//     0.001));
//   }
//   delete[] input;
//   delete[] output;
// }
TEST_CASE("Par 4 alligned solution - Test", "[par4_all]") {
  Conditions conditions = {1, 0.5, 0.1, 16, 30};
  float* input          = new float[conditions.n_x];
  float* output         = new float[conditions.n_x];
  initialize_array(input, conditions.n_x);
  initialize_array(output, conditions.n_x);
  input[0]                  = 100;
  input[conditions.n_x - 1] = 200;
  parallel4_alligned(conditions, input, output);
  for (int i = 0; i < conditions.n_x; i++) {
    REQUIRE_THAT(output[i], Catch::Matchers::WithinAbs(expected[i], 0.001));
  }
  delete[] input;
  delete[] output;
}

TEST_CASE("Sequential Unroll solution - Test", "[seq_unr]") {
  Conditions conditions = {1, 0.5, 0.1, 16, 30};
  float* input          = new float[conditions.n_x];
  float* output         = new float[conditions.n_x];
  initialize_array(input, conditions.n_x);
  initialize_array(output, conditions.n_x);
  input[0]                  = 100;
  input[conditions.n_x - 1] = 200;
  sequential_unroll(conditions, input, output);
  for (int i = 0; i < conditions.n_x; i++) {
    // fmt::print("{} ", output[i]);
    REQUIRE_THAT(output[i], Catch::Matchers::WithinAbs(expected[i], 0.001));
  }
  delete[] input;
  delete[] output;
}
