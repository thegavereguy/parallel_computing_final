#include <fmt/base.h>
#include <lib/distributed.h>
#include <mpi.h>

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/benchmark/catch_chronometer.hpp>
#include <catch2/benchmark/catch_clock.hpp>
#include <catch2/catch_get_random_seed.hpp>
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_case_info.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/reporters/catch_reporter_registrars.hpp>
#include <catch2/reporters/catch_reporter_streaming_base.hpp>
#include <cstdint>
#include <cstdio>

TEST_CASE("GPU 256 solution", "[gpu256]") {
  char* name = new char[100];

  for (Conditions conditions : test_cases) {
    sprintf(name, "%ld,%ld", (long)conditions.n_x, (long)conditions.n_t);

    BENCHMARK_ADVANCED(name)(Catch::Benchmark::Chronometer meter) {
      meter.measure([] {});
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
    fmt::print("NX,NT,MEAN,MINT,MAXT,ITER\n");
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
