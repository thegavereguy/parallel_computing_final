add_executable(cpu_bench cpu_bench.cpp)
add_executable(cpu_tests cpu_tests.cpp)

target_link_libraries(cpu_bench PUBLIC fmt Catch2::Catch2 shared )
target_link_libraries(cpu_tests PUBLIC fmt Catch2::Catch2WithMain shared common )


if(DEFINED ENV{BUILD_VULKAN} AND "$ENV{BUILD_VULKAN}" STREQUAL "true")
	add_executable(gpu_bench gpu_bench.cpp)
	target_link_libraries(gpu_bench PUBLIC fmt Catch2::Catch2WithMain shared framework)
	add_executable(gpu_tests gpu_tests.cpp)
	target_link_libraries(gpu_tests PUBLIC fmt Catch2::Catch2WithMain shared common framework)
endif()
