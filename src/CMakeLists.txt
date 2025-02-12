add_library(test_lib STATIC test.cpp ../include/lib/test.h)
add_library(shared STATIC shared.cpp ../include/lib/shared.h)
add_library(common STATIC common.cpp ../include/lib/common.h)

target_include_directories(test_lib PUBLIC ../include)
target_include_directories(shared PUBLIC ../include)
target_include_directories(common PUBLIC ../include)

target_link_libraries(test_lib PUBLIC Catch2::Catch2 fmt)

find_package(OpenMP REQUIRED)
target_link_libraries(shared PUBLIC fmt OpenMP::OpenMP_CXX)

target_compile_options(shared PUBLIC -fopenmp)

# check variable BUILD_OPTIMIZED
if(DEFINED ENV{BUILD_OPTIMIZED} AND "$ENV{BUILD_OPTIMIZED}" STREQUAL "true")
	message("Building optimized version of shared lib")
	target_compile_options(shared PUBLIC -O3)
else()
	message("Building non-optimized version of shared lib")
	target_compile_options(shared PUBLIC -O0)
endif()


if(DEFINED ENV{BUILD_MPI} AND "$ENV{BUILD_MPI}" STREQUAL "true")

	find_package(MPI REQUIRED)
	add_library(distributed STATIC distributed.cpp ../include/lib/distributed.h)
	target_link_libraries(distributed PUBLIC MPI::MPI_CXX fmt)
	target_include_directories(distributed PUBLIC ../include)
	target_include_directories(distributed SYSTEM PUBLIC ${MPI_INCLUDE_PATH})
endif()
