#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <deque>
#include <functional>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
// #include <vulkan/vulkan_to_string.hpp>

#define VK_CHECK(x)                          \
  do {                                       \
    VkResult err = x;                        \
    if (err) {                               \
      fmt::print("Detected Vulkan error: "); \
      abort();                               \
    }                                        \
  } while (0)

// macro that takes bUseValidationLayers as a parameter and prints only if true
#define VALIDATION_MESSAGE(message) \
  if (_useValidationLayers) {       \
    fmt::print(message);            \
  }

struct DeletionQueue {
  std::deque<std::function<void()>> deletors;
  void push(std::function<void()>&& function);
  void flush();
};

struct FrameData {  // holds the structures and commands needed to render a
                    // frame
  VkCommandPool _commandPool;
  VkCommandBuffer _mainCommandBuffer;
  VkSemaphore _swapchainSemaphore;  // needed so that the render commands wait
                                    // on the swapchain image request.
  VkSemaphore _renderSemaphore;  // used to control presenting the image to the
                                 // os once the drawing finishes
  VkFence _renderFence;  // Allows to wait for the GPU to finish rendering

  DeletionQueue _deleteQueue;
};

struct AllocatedImage {
  VkImage image;
  VkImageView imageView;
  VmaAllocation allocation;
  VkExtent3D imageExtent;
  VkFormat imageFormat;
};
