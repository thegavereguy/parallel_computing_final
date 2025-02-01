#pragma once

#include <VkBootstrap.h>
#include <framework/vk_descriptors.h>
#include <framework/vk_types.h>
#include <vulkan/vulkan_core.h>

constexpr unsigned int FRAME_OVERLAP = 2;
class VulkanEngine {
 public:
  bool _isInitialized{false};
  int _frameNumber{0};
  bool stop_rendering{false};

  VkInstance _instance;                       // Vulkan library handle
  VkDebugUtilsMessengerEXT _debug_messenger;  // Vulkan debug output handle

  VkSurfaceKHR _surface;  // this is going to be a headless surface provided by
                          // the extension VK_EXT_headless_surface
  VkPhysicalDevice _chosenGPU;  // GPU chosen as the default device
  VkDevice _device;             // Vulkan device for commands

  VkQueue _graphicsQueue;
  uint32_t _graphicsQueueFamily;

  DeletionQueue _mainDeletionQueue;

  VmaAllocator _allocator;
  VkCommandPool _commandPool;
  VkCommandBuffer _mainCommandBuffer;
  VkSemaphore _swapchainSemaphore;  // needed so that the render commands wait
                                    // on the swapchain image request.
  VkSemaphore _renderSemaphore;  // used to control presenting the image to the
                                 // os once the drawing finishes
  VkFence _renderFence;  // Allows to wait for the GPU to finish rendering

  DescriptorAllocator globalDescriptorAllocator;
  VkDescriptorSet _drawImageDescriptors;
  VkDescriptorSetLayout _computeDescriptorLayout;

  VkPipeline _computePipeline;
  VkPipelineLayout pipelineLayout;

  VkBuffer _inputBuffer;
  VkBuffer _outputBuffer;
  VmaAllocation _inputBufferMemory;
  VmaAllocation _outputBufferMemory;

  uint32_t _gridSize;
  float _dt, _dx, _alpha;

  struct PushConstants {
    float dt;
    float dx;
    float alpha;
    uint32_t size;
  } _pushConstants;

  static VulkanEngine& Get();

  // initializes everything in the engine
  void init(bool);

  // shuts down the engine
  void cleanup();

  // run main loop
  void run();

  void compute();
  void run_compute(const std::vector<float>& initial_conditions,
                   uint32_t timesteps);
  void set_costants(float dt, float dx, float alpha, uint32_t size);

 private:
  void init_vulkan();
  void init_commands();
  void init_buffers();
  void init_sync_structures();
  void init_descriptors();
  void init_pipelines();
  void init_background_pipelines();
};
