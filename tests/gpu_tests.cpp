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
  for (int i = 0; i < conditions.n_x; i++) {
    // fmt::print("{} ", output[i]);
    REQUIRE_THAT(output[i], Catch::Matchers::WithinAbs(expected[i], 0.001));
  }

  engine.cleanup();
};
