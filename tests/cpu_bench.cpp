#include <fmt/base.h>
#include <lib/shared.h>

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/benchmark/catch_chronometer.hpp>
#include <catch2/benchmark/catch_clock.hpp>
#include <catch2/catch_get_random_seed.hpp>
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_case_info.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/reporters/catch_reporter_registrars.hpp>
#include <catch2/reporters/catch_reporter_streaming_base.hpp>

TEST_CASE("Sequential solution", "[cpu_seq]") {
  char* name = new char[100];
  for (Conditions conditions : test_cases) {
    sprintf(name, "%ld", (long)conditions.n_x * (long)conditions.n_t);

    BENCHMARK_ADVANCED(name)(Catch::Benchmark::Chronometer meter) {
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
  }
}
TEST_CASE("Parallel 2 inner solution", "[cpu_par2]") {
  char* name = new char[100];
  for (Conditions conditions : test_cases) {
    sprintf(name, "%ld", (long)conditions.n_x * (long)conditions.n_t);

    BENCHMARK_ADVANCED(name)(Catch::Benchmark::Chronometer meter) {
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
}
TEST_CASE("Parallel 4 inner solution", "[cpu_par4]") {
  char* name = new char[100];
  for (Conditions conditions : test_cases) {
    sprintf(name, "%ld", (long)conditions.n_x * (long)conditions.n_t);

    BENCHMARK_ADVANCED(name)(Catch::Benchmark::Chronometer meter) {
      float* input              = new float[conditions.n_x];
      input[0]                  = 100;
      input[conditions.n_x - 1] = 200;
      float* output             = new float[conditions.n_x];

      meter.measure([conditions, input, output] {
        return parallel4_inner(conditions, input, output);
      });

      delete[] input;
      delete[] output;
    };
  }
}
TEST_CASE("Parallel 8 inner solution", "[cpu_par8]") {
  char* name = new char[100];
  for (Conditions conditions : test_cases) {
    sprintf(name, "%ld", (long)conditions.n_x * (long)conditions.n_t);

    BENCHMARK_ADVANCED(name)(Catch::Benchmark::Chronometer meter) {
      float* input              = new float[conditions.n_x];
      input[0]                  = 100;
      input[conditions.n_x - 1] = 200;
      float* output             = new float[conditions.n_x];

      meter.measure([conditions, input, output] {
        return parallel8_inner(conditions, input, output);
      });

      delete[] input;
      delete[] output;
    };
  }
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
    // print the first 4 decimal places
    fmt::print("{},{:.4f},{:.4f},{:.4f},{}\n", stats.info.name,
               stats.mean.point.count() / 1e6,
               stats.mean.lower_bound.count() / 1e6,
               stats.mean.upper_bound.count() / 1e6, stats.info.iterations);
  }
};
CATCH_REGISTER_REPORTER("csv", PartialCSVReporter)
