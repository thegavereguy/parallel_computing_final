cmake_minimum_required(VERSION 3.20)
project(HelloTriangle CXX)

set(CMAKE_CXX_STANDARD 20)

#download the vulkan sdk and execute the script to set the environment variables
Include(FetchContent)



add_subdirectory(third_party)
add_subdirectory(framework)
add_subdirectory(apps)
add_subdirectory(shaders)
add_subdirectory(src)
add_subdirectory(tests)
