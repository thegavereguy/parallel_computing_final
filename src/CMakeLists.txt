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
