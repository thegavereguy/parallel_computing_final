cmake_minimum_required(VERSION 3.10)

project(framework LANGUAGES C CXX)


set(FRAMEWORK_HEADERS
	include/vk_types.h
	include/vk_engine.h
	include/vk_initializers.h
	include/vk_image.h
	include/vk_descriptors.h
	include/vk_pipelines.h
)

	set(FRAMEWORK_SOURCES
	vk_engine.cpp
	vk_initializers.cpp
	vk_image.cpp
	vk_descriptors.cpp
	vk_pipelines.cpp
)
set(PROJECT_FILES ${FRAMEWORK_headers} ${FRAMEWORK_SOURCES})

add_library(framework STATIC ${PROJECT_FILES})
target_include_directories(framework PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

find_package(Vulkan REQUIRED)

target_link_libraries(framework PUBLIC glm vma vk-bootstrap Vulkan::Vulkan SDL2::SDL2-static)
target_link_libraries(framework PUBLIC fmt)
