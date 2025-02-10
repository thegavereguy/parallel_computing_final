#include <fmt/base.h>
#include <framework/vk_engine.h>
#include <lib/common.h>

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/benchmark/catch_chronometer.hpp>
#include <catch2/benchmark/catch_clock.hpp>
#include <catch2/catch_get_random_seed.hpp>
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_case_info.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/reporters/catch_reporter_registrars.hpp>
#include <catch2/reporters/catch_reporter_streaming_base.hpp>
#include <cstdio>

TEST_CASE("GPU1 solution", "[gpugrid]") {
  char* name = new char[100];
  for (Conditions conditions : test_cases) {
    sprintf(name, "%ld", (long)conditions.n_x * (long)conditions.n_t);

    BENCHMARK_ADVANCED(name)(Catch::Benchmark::Chronometer meter) {
      std::vector<float> input  = std::vector<float>(conditions.n_x);
      input[0]                  = 100;
      input[conditions.n_x - 1] = 200;
      std::vector<float> output = std::vector<float>(conditions.n_x);
      double dx                 = conditions.L / (conditions.n_x - 1);
      double dt                 = conditions.t_final / (conditions.n_t - 1);

      // fmt::print("{}", conditions.alpha * (dt / (dx * dx)));
      VulkanEngine engine;
      engine.set_costants(dt, dx, conditions.alpha, conditions.n_x);
      engine.init(false);
      engine.set_initial_conditions(input);

      int compute_groups =
          conditions.n_x / 1024 == 0 ? 1 : conditions.n_x / 1024;

      meter.measure([conditions, input, &engine, compute_groups] {
        return engine.run_compute(conditions.n_t, compute_groups);
      });
      engine.cleanup();
    };
  }
  delete[] name;
}

int main(int argc, char* argv[]) {
  int result = Catch::Session().run(argc, argv);
  return result;
}

class PartialCSVReporter : public Catch::StreamingReporterBase {
 public:
  using StreamingReporterBase::StreamingReporterBase;

  static std::string getDescription() {
    return "Reporter for benchmarks in CSV format";
  }

  void testCasePartialStarting(Catch::TestCaseInfo const& testInfo,
                               uint64_t partNumber) override {
    // std::cout << "TestCase: " << testInfo.name << '#' << partNumber << '\n';
    // std::cout << "DIMENSION,MEAN,MINT,MAXT,ITER" << '\n';
    fmt::print("DIMENSION,MEAN,MINT,MAXT,ITER\n");
  }

  void testCasePartialEnded(Catch::TestCaseStats const& testCaseStats,
                            uint64_t partNumber) override {
    // std::cout << "TestCaseEnded: " << testCaseStats.testInfo->name << '#' <<
    // partNumber << '\n';
  }

  void benchmarkEnded(Catch::BenchmarkStats<> const& stats) override {
    // std::cout << stats.info.name << "," << stats.mean.point.count() / 1e6 <<
    // ","
    //           << stats.mean.lower_bound.count() / 1e6 << ","
    //           << stats.mean.upper_bound.count() / 1e6 << ","
    //           << stats.info.iterations << '\n';
    fmt::print("{},{:.4f},{:.4f},{:.4f},{}\n", stats.info.name,
               stats.mean.point.count() / 1e6,
               stats.mean.lower_bound.count() / 1e6,
               stats.mean.upper_bound.count() / 1e6, stats.info.iterations);
  }
};
CATCH_REGISTER_REPORTER("csv", PartialCSVReporter)
