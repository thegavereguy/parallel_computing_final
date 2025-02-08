add_executable(benchmarks benchmarks.cpp)
add_executable(tests tests.cpp)

target_link_libraries(benchmarks PUBLIC fmt Catch2::Catch2 shared framework)
target_link_libraries(tests PUBLIC fmt Catch2::Catch2WithMain shared common framework)
