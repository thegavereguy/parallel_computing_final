cmake_minimum_required(VERSION 3.20)
project(HelloTriangle CXX)

set(CMAKE_CXX_STANDARD 20)

Include(FetchContent)

add_subdirectory(third_party)

#check if the environment variable BUILD_VULKAN is set, if yes, build the vulkan backend
if(DEFINED ENV{BUILD_VULKAN} AND "$ENV{BUILD_VULKAN}" STREQUAL "true")
	Message("Building Vulkan backend")
	add_subdirectory(framework)
	add_subdirectory(shaders)
else()
	Message("Skipping Vulkan backend")
endif()

add_subdirectory(src)
add_subdirectory(apps)
add_subdirectory(tests)
