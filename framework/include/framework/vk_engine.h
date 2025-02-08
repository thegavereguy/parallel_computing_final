#pragma once

#include <VkBootstrap.h>
#include <framework/vk_descriptors.h>
#include <framework/vk_types.h>
#include <vulkan/vulkan_core.h>

#include <vector>

constexpr unsigned int FRAME_OVERLAP = 2;
class VulkanEngine {
 public:
  bool _isInitialized{false};
  int _frameNumber{0};
  bool stop_rendering{false};
  bool _useValidationLayers;

  VkInstance _instance;                       // Vulkan library handle
  VkDebugUtilsMessengerEXT _debug_messenger;  // Vulkan debug output handle

  VkSurfaceKHR _surface;  // this is going to be a headless surface provided by
                          // the extension VK_EXT_headless_surface
  VkPhysicalDevice _chosenGPU;  // GPU chosen as the default device
  VkPhysicalDeviceLimits _limits;
  VkPhysicalDeviceProperties _properties;
  VkDevice _device;  // Vulkan device for commands

  VkQueue _graphicsQueue;
  VkQueue _computeQueue;
  uint32_t _graphicsQueueFamily;
  uint32_t _computeQueueFamily;

  DeletionQueue _mainDeletionQueue;

  VmaAllocator _allocator;
  VkCommandPool _commandPool;
  VkCommandBuffer _mainCommandBufferA;
  VkCommandBuffer _mainCommandBufferB;
  VkSemaphore _swapchainSemaphore;  // needed so that the render commands wait
                                    // on the swapchain image request.
  VkSemaphore _renderSemaphore;  // used to control presenting the image to the
                                 // os once the drawing finishes
  VkFence _renderFence;  // Allows to wait for the GPU to finish rendering

  DescriptorAllocator globalDescriptorAllocator;
  VkDescriptorSet _bufferDescriptorsA;
  VkDescriptorSet _bufferDescriptorsB;
  VkDescriptorSetLayout _computeDescriptorLayout;

  VkPipeline _computePipeline;
  VkPipelineLayout pipelineLayout;

  VkBuffer _inputBuffer;
  VkBuffer _outputBuffer;
  VmaAllocation _inputBufferAlloc;
  VmaAllocation _outputBufferAlloc;

  uint32_t _gridSize;
  float _dt, _dx, _alpha;
  float* _initial_conditions;

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
  std::vector<float> run_compute(uint32_t timesteps, uint32_t groupCount);
  void set_costants(float dt, float dx, float alpha, uint32_t size);
  void set_initial_conditions(std::vector<float>);

 private:
  void init_vulkan();
  void init_commands();
  void init_buffers();
  void init_sync_structures();
  void init_descriptors();
  void init_pipelines();
  void init_background_pipelines();
  void write_buffer();
  std::vector<float> read_buffer();
};
