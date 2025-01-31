#pragma once

#include <VkBootstrap.h>
#include <framework/vk_descriptors.h>
#include <framework/vk_types.h>
#include <vulkan/vulkan_core.h>

#include <functional>

constexpr unsigned int FRAME_OVERLAP = 2;
class VulkanEngine {
 public:
  bool _isInitialized{false};
  int _frameNumber{0};
  bool stop_rendering{false};
  VkExtent2D _windowExtent{480, 480};

  struct SDL_Window* _window{nullptr};

  VkInstance _instance;                       // Vulkan library handle
  VkDebugUtilsMessengerEXT _debug_messenger;  // Vulkan debug output handle

  VkPhysicalDevice _chosenGPU;  // GPU chosen as the default device
  VkDevice _device;             // Vulkan device for commands
  VkSurfaceKHR _surface;        // Vulkan window surface

  VkSwapchainKHR _swapchain;       // The vk swapchain reference
  VkFormat _swapchainImageFormat;  // The format of the swapchain images

  std::vector<VkImage> _swapchainImages;  // Handler to the images to use as a
                                          // texture or to render into
  std::vector<VkImageView> _swapchainImageViews;  // A wrapper for the images
  VkExtent2D _swapchainExtent;

  FrameData _frames[FRAME_OVERLAP];

  VkQueue _graphicsQueue;
  uint32_t _graphicsQueueFamily;

  DeletionQueue _mainDeletionQueue;

  VmaAllocator _allocator;

  AllocatedImage _drawImage;
  VkExtent2D _drawExtent;

  DescriptorAllocator globalDescriptorAllocator;
  VkDescriptorSet _drawImageDescriptors;
  VkDescriptorSetLayout _drawImageDescriptorLayout;

  VkPipeline _gradientPipeline;
  VkPipelineLayout _gradientPipelineLayout;

  static VulkanEngine& Get();

  // initializes everything in the engine
  void init();

  // shuts down the engine
  void cleanup();

  // draw loop
  void draw();
  void draw_background(VkCommandBuffer cmd);

  // run main loop
  void run();

  FrameData& get_current_frame() {
    return _frames[_frameNumber % FRAME_OVERLAP];
  };

 private:
  void init_vulkan();
  void init_swapchain();
  void init_commands();
  void init_sync_structures();
  void create_swapchain(uint32_t width, uint32_t height);
  void destroy_swapchain();
  void init_descriptors();
  void init_pipelines();
  void init_background_pipelines();
};
