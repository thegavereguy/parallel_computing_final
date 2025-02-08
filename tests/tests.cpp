#include <fmt/base.h>
#include <framework/vk_engine.h>
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
  Conditions conditions = {1, 0.5, 0.1, 16, 30};
  float* input          = new float[conditions.n_x];
  float* output         = new float[conditions.n_x];
  initialize_array(input, conditions.n_x);
  initialize_array(output, conditions.n_x);

  input[0]                  = 100;
  input[conditions.n_x - 1] = 200;

  fmt::print("Sequential solution {}\n", conditions.alpha);
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

  fmt::print("par2 solution{}\n", conditions.alpha);
  sequential(conditions, input, output);
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

TEST_CASE("Parallel2 outer solution - Test", "[par2_out]") {
  Conditions conditions = {1, 0.5, 0.1, 16, 30};
  float* input          = new float[conditions.n_x];
  float* output         = new float[conditions.n_x];
  initialize_array(input, conditions.n_x);
  initialize_array(output, conditions.n_x);
  input[0]                  = 100;
  input[conditions.n_x - 1] = 200;

  parallel2_outer(conditions, input, output);
  for (int i = 0; i < conditions.n_x; i++) {
    REQUIRE_THAT(output[i], Catch::Matchers::WithinAbs(expected[i], 0.001));
  }
  delete[] input;
  delete[] output;
}
TEST_CASE("GPU1 solution", "[gpu1]") {
  Conditions conditions = {1, 0.5, 0.1, 16, 30};

  std::vector<float> input  = std::vector<float>(conditions.n_x);
  input[0]                  = 100;
  input[conditions.n_x - 1] = 200;
  std::vector<float> output = std::vector<float>(conditions.n_x);
  double dx                 = conditions.L / (conditions.n_x - 1);
  double dt                 = conditions.t_final / (conditions.n_t - 1);

  VulkanEngine engine;
  engine.set_costants(dt, dx, conditions.alpha, conditions.n_x);
  engine.init(false);
  engine.set_initial_conditions(input);

  output = engine.run_compute(conditions.n_t, 1);

  for (auto val : output) {
    fmt::print("{} ", val);
  }
  engine.cleanup();
};
