cmake_minimum_required(VERSION 3.20)
project(HelloTriangle CXX)

set(CMAKE_CXX_STANDARD 20)

find_package(Vulkan REQUIRED)

add_subdirectory(third_party)
add_subdirectory(framework)

add_subdirectory(src)
