#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <array>
#include <deque>
#include <functional>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <vector>
// #include <vulkan/vulkan_to_string.hpp>

#define VK_CHECK(x)                          \
  do {                                       \
    VkResult err = x;                        \
    if (err) {                               \
      fmt::print("Detected Vulkan error: "); \
      abort();                               \
    }                                        \
  } while (0)
