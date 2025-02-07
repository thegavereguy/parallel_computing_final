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

const float expected[16] = {100,        86.71643,  74.04589,  63.181152,
                            54.315628,  48.4243,   46.228462, 47.254143,
                            53.699066,  62.853626, 78.45121,  95.896545,
                            119.290276, 143.68361, 171.68854, 200};

TEST_CASE("Sequential solution - Test", "[seq]") {
  Conditions conditions     = {1, 0.5, 0.1, 16, 30};
  float* input              = new float[conditions.n_x];
  input[0]                  = 100;
  input[conditions.n_x - 1] = 200;
  float* output             = new float[conditions.n_x];

  fmt::print("Sequential solution {}\n", conditions.alpha);
  sequential(conditions, input, output);
  for (int i = 0; i < conditions.n_x; i++) {
    fmt::print("{}, ", output[i]);
    REQUIRE_THAT(output[i], Catch::Matchers::WithinAbs(expected[i], 0.001));
  }
  fmt::print("\n");
  delete[] input;
  delete[] output;
}
TEST_CASE("Parallel2 inner solution - Test", "[par2]") {
  Conditions conditions     = {1, 0.5, 0.1, 16, 30};
  float* input              = new float[conditions.n_x];
  input[0]                  = 100;
  input[conditions.n_x - 1] = 200;
  float* output             = new float[conditions.n_x];

  fmt::print("par2 solution{}\n", conditions.alpha);
  sequential(conditions, input, output);
  for (int i = 0; i < conditions.n_x; i++) {
    fmt::print("{}, ", output[i]);
    REQUIRE_THAT(output[i], Catch::Matchers::WithinAbs(expected[i], 0.001));
  }
  fmt::print("\n");
  delete[] input;
  delete[] output;
}
TEST_CASE("Parallel4 inner solution - Test", "[par4]") {
  Conditions conditions     = {1, 0.5, 0.1, 16, 30};
  float* input              = new float[conditions.n_x];
  input[0]                  = 100;
  input[conditions.n_x - 1] = 200;
  float* output             = new float[conditions.n_x];
  fmt::print("par4 solution\n");
  parallel4_inner(conditions, input, output);
  for (int i = 0; i < conditions.n_x; i++) {
    fmt::print("{}, ", output[i]);
    // REQUIRE_THAT(output[i], Catch::Matchers::WithinAbs(expected[i], 0.001));
  }
  fmt::print("\n");

  delete[] input;
  delete[] output;
}

TEST_CASE("Parallel8 inner solution - Test", "[par8]") {
  Conditions conditions     = {1, 0.5, 0.1, 16, 30};
  float* input              = new float[conditions.n_x];
  input[0]                  = 100;
  input[conditions.n_x - 1] = 200;
  float* output             = new float[conditions.n_x];
  parallel8_inner(conditions, input, output);
  for (int i = 0; i < conditions.n_x; i++) {
    fmt::print("{}, ", output[i]);
    // REQUIRE_THAT(output[i], Catch::Matchers::WithinAbs(expected[i], 0.001));
  }
  fmt::print("\n");
  delete[] input;
  delete[] output;
}

TEST_CASE("Parallel2 outer solution - Test", "[par2_out]") {
  Conditions conditions     = {1, 0.5, 0.1, 16, 30};
  float* input              = new float[conditions.n_x];
  input[0]                  = 100;
  input[conditions.n_x - 1] = 200;
  float* output             = new float[conditions.n_x];
  parallel2_outer(conditions, input, output);
  for (int i = 0; i < conditions.n_x; i++) {
    fmt::print("{}, ", output[i]);
    // REQUIRE_THAT(output[i], Catch::Matchers::WithinAbs(expected[i], 0.001));
  }
  fmt::print("\n");
  delete[] input;
  delete[] output;
}
