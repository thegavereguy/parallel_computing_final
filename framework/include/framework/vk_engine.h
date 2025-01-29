#pragma once

#include <VkBootstrap.h>
#include <framework/vk_types.h>

struct FrameData {  // holds the structures and commands needed to render a
                    // frame
  VkCommandPool _commandPool;
  VkCommandBuffer _mainCommandBuffer;
  VkSemaphore _swapchainSemaphore;  // needed so that the render commands wait
                                    // on the swapchain image request.
  VkSemaphore _renderSemaphore;  // used to control presenting the image to the
                                 // os once the drawing finishes
  //
  VkFence _renderFence;  // Allows to wait for the GPU to finish rendering
};

constexpr unsigned int FRAME_OVERLAP = 2;
class VulkanEngine {
 public:
  bool _isInitialized{false};
  int _frameNumber{0};
  bool stop_rendering{false};
  VkExtent2D _windowExtent{1700, 900};

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

  static VulkanEngine& Get();

  // initializes everything in the engine
  void init();

  // shuts down the engine
  void cleanup();

  // draw loop
  void draw();

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
};
