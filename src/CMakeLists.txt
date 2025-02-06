add_library(test_lib STATIC test.cpp ../include/lib/test.h)

target_include_directories(test_lib PUBLIC ../include)
target_link_libraries(test_lib PUBLIC Catch2::Catch2 fmt)
