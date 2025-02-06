cmake_minimum_required(VERSION 3.10)


project(third_party LANGUAGES C CXX)

add_subdirectory(fmt)

# glm
add_library(glm INTERFACE)
set(GLM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/glm)
target_sources(glm INTERFACE ${GLM_DIR}/glm/glm.hpp)
target_include_directories(glm SYSTEM INTERFACE ${GLM_DIR})

# vma
add_library(vma INTERFACE)
set(VMA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/VulkanMemoryAllocator/include)
target_sources(vma INTERFACE ${VMA_DIR}/vk_mem_alloc.h)
target_include_directories(vma SYSTEM INTERFACE ${VMA_DIR})

# vk-bootstrap
add_library(vk-bootstrap INTERFACE)
set(VK_BOOTSTRAP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vk-bootstrap/src)
target_sources(vk-bootstrap INTERFACE ${VK_BOOTSTRAP_DIR}/VkBootstrap.h ${VK_BOOTSTRAP_DIR}/VkBootstrap.cpp ${VK_BOOTSTRAP_DIR}/VkBootstrapDispatch.h)
target_include_directories(vk-bootstrap SYSTEM INTERFACE ${VK_BOOTSTRAP_DIR})

add_subdirectory(Catch2)
