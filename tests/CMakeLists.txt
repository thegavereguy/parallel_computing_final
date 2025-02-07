add_executable(benchmarks benchmarks.cpp)

find_package(Catch2 REQUIRED)
target_link_libraries(benchmarks PUBLIC fmt Catch2::Catch2WithMain shared)
