#include <fmt/base.h>
#include <lib/shared.h>

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/benchmark/catch_chronometer.hpp>
#include <catch2/benchmark/catch_clock.hpp>
#include <catch2/catch_get_random_seed.hpp>
#include <catch2/catch_test_case_info.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/reporters/catch_reporter_registrars.hpp>
#include <catch2/reporters/catch_reporter_streaming_base.hpp>

TEST_CASE("Sequential solution", "[seq]") {
  BENCHMARK_ADVANCED("Seq")(Catch::Benchmark::Chronometer meter) {
    Conditions conditions = {1, 0.02, 1, 262144, 10000};

    float* input              = new float[conditions.n_x];
    input[0]                  = 100;
    input[conditions.n_x - 1] = 200;
    float* output             = new float[conditions.n_x];

    meter.measure([conditions, input, output] {
      return sequential(conditions, input, output);
    });

    delete[] input;
    delete[] output;
  };
  BENCHMARK_ADVANCED("Par2 inner")(Catch::Benchmark::Chronometer meter) {
    Conditions conditions = {1, 0.02, 1, 262144, 10000};

    float* input              = new float[conditions.n_x];
    input[0]                  = 100;
    input[conditions.n_x - 1] = 200;
    float* output             = new float[conditions.n_x];

    meter.measure([conditions, input, output] {
      return parallel2_inner(conditions, input, output);
    });

    delete[] input;
    delete[] output;
  };
  BENCHMARK_ADVANCED("Par4 inner")(Catch::Benchmark::Chronometer meter) {
    Conditions conditions = {1, 0.02, 1, 262144, 10000};

    float* input              = new float[conditions.n_x];
    input[0]                  = 100;
    input[conditions.n_x - 1] = 200;
    float* output             = new float[conditions.n_x];

    meter.measure([conditions, input, output] {
      return parallel2_inner(conditions, input, output);
    });

    delete[] input;
    delete[] output;
  };
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
    fmt::print("{},{},{},{},{}\n", stats.info.name,
               stats.mean.point.count() / 1e6,
               stats.mean.lower_bound.count() / 1e6,
               stats.mean.upper_bound.count() / 1e6, stats.info.iterations);
  }
};
CATCH_REGISTER_REPORTER("csv", PartialCSVReporter)
