cmake_minimum_required(VERSION 3.10)

project(framework LANGUAGES C CXX)

if(DEFINED ENV{DOWNLOAD_VULKAN_SDK} AND "$ENV{DOWNLOAD_VULKAN_SDK}" STREQUAL "true")
# Message("Downloading Vulkan SDK" )
# FetchContent_Declare(
# 		VulkanSDK
# 		URL https://sdk.lunarg.com/sdk/download/1.3.296.0/linux/vulkansdk-linux-x86_64-1.3.296.0.tar.xz 
# 		DOWNLOAD_EXTRACT_TIMESTAMP true
#
# )
# FetchContent_MakeAvailable(VulkanSDK)
# add_custom_command(
# 		OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/VulkanSDK
# 		COMMAND ${CMAKE_COMMAND} -E tar xvf ${CMAKE_CURRENT_BINARY_DIR}/VulkanSDK
# 		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
# )
# add_custom_target(
# 		ExtractVulkanSDK
# 		DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/VulkanSDK
# )

set(VULKAN_SDK_DIR "${CMAKE_BINARY_DIR}/VulkanSDK")


if(WIN32)
	message("Downloading Vulkan SDK for Windows")
		set(VULKAN_SDK_URL "https://sdk.lunarg.com/sdk/download/latest/windows/vulkan-sdk.exe")
		set(VULKAN_SDK_INSTALLER "${CMAKE_BINARY_DIR}/vulkan-sdk.exe")
    execute_process(
        COMMAND ${VULKAN_SDK_INSTALLER} /S
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
elseif(UNIX)
	message("Downloading Vulkan SDK for Linux")
	set(VULKAN_SDK_VER "1.3.296.0")
		set(VULKAN_SDK_TAR "vulkansdk-linux-x86_64-${VULKAN_SDK_VER}.tar.xz")
		message(source  ${VULKAN_SDK_DIR}/${VULKAN_SDK_VER}/setup-env.sh)
	file(DOWNLOAD "https://sdk.lunarg.com/sdk/download/${VULKAN_SDK_VER}/linux/vulkansdk-linux-x86_64-${VULKAN_SDK_VER}.tar.xz" ${CMAKE_BINARY_DIR}/${VULKAN_SDK_TAR} SHOW_PROGRESS CACHE)

		execute_process(COMMAND mkdir -p ${VULKAN_SDK_DIR})
    execute_process(
        COMMAND tar -xvf ${CMAKE_BINARY_DIR}/${VULKAN_SDK_TAR} -C ${VULKAN_SDK_DIR}
    )
	execute_process(COMMAND source ${VULKAN_SDK_DIR}/${VULKAN_SDK_VER}/setup-env.sh)
	set(ENV{VULKAN_SDK} "${VULKAN_SDK_DIR}/${VULKAN_SDK_VER}")
endif()

endif()

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
target_include_directories(framework PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/VulkanSDK/include)

find_package(Vulkan REQUIRED)

target_link_libraries(framework PUBLIC glm vma vk-bootstrap Vulkan::Vulkan )
target_link_libraries(framework PUBLIC fmt)
