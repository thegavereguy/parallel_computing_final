#include <SDL.h>
#include <SDL_vulkan.h>
#include <fmt/core.h>
#include <framework/vk_engine.h>
#include <framework/vk_initializers.h>
#include <framework/vk_types.h>
#include <vulkan/vulkan_core.h>

#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <thread>

#include "fmt/base.h"
#include "framework/vk_image.h"

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

constexpr bool bUseValidationLayers = false;

VulkanEngine* loadedEngine = nullptr;

VulkanEngine& VulkanEngine::Get() { return *loadedEngine; }
void VulkanEngine::init() {
  // only one engine initialization is allowed with the application.
  assert(loadedEngine == nullptr);
  loadedEngine = this;

  // We initialize SDL and create a window with it.
  SDL_Init(SDL_INIT_VIDEO);

  SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);

  _window = SDL_CreateWindow("Vulkan Engine", SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED, _windowExtent.width,
                             _windowExtent.height, window_flags);

  init_vulkan();
  init_swapchain();
  init_commands();
  init_sync_structures();

  _isInitialized = true;
}
void VulkanEngine::cleanup() {
  if (_isInitialized) {
    // changing the order of distructions can cause errors
    fmt::print("Cleaning up Vulkan Engine\n");
    vkDeviceWaitIdle(_device);

    for (int i = 0; i < FRAME_OVERLAP; i++) {
      vkDestroyCommandPool(_device, _frames[i]._commandPool, nullptr);

      // destroy synchronization objects
      vkDestroyFence(_device, _frames[i]._renderFence, nullptr);
      vkDestroySemaphore(_device, _frames[i]._renderSemaphore, nullptr);
      vkDestroySemaphore(_device, _frames[i]._swapchainSemaphore, nullptr);
      _frames[i]._deleteQueue.flush();
    }

    _mainDeletionQueue.flush();

    destroy_swapchain();
    vkDestroySurfaceKHR(_instance, _surface, nullptr);
    vkDestroyDevice(_device, nullptr);

    vkb::destroy_debug_utils_messenger(_instance, _debug_messenger);
    vkDestroyInstance(_instance, nullptr);

    SDL_DestroyWindow(_window);
    loadedEngine = nullptr;
  }
}

void VulkanEngine::run() {
  SDL_Event e;
  bool bQuit = false;

  // main loop
  while (!bQuit) {
    // Handle events on queue
    while (SDL_PollEvent(&e) != 0) {
      // close the window when user alt-f4s or clicks the X button
      if (e.type == SDL_QUIT) bQuit = true;

      if (e.type == SDL_WINDOWEVENT) {
        if (e.window.event == SDL_WINDOWEVENT_MINIMIZED) {
          stop_rendering = true;
        }
        if (e.window.event == SDL_WINDOWEVENT_RESTORED) {
          stop_rendering = false;
        }
      }
    }

    // do not draw if we are minimized
    if (stop_rendering) {
      // throttle the speed to avoid the endless spinning
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      continue;
    }

    draw();
  }
}

void VulkanEngine::draw() {
  VK_CHECK(vkWaitForFences(
      _device, 1, &_frames[_frameNumber % FRAME_OVERLAP]._renderFence, VK_TRUE,
      10000000000));  // wait for the GPU to have finished the render, the
  get_current_frame()._deleteQueue.flush();
  // timeout is in nanoseconds
  VK_CHECK(
      vkResetFences(_device, 1,
                    &_frames[_frameNumber % FRAME_OVERLAP]
                         ._renderFence));  // reset the fence for next frame
  // request image from the swapchain
  uint32_t swapchainImageIndex;
  VK_CHECK(vkAcquireNextImageKHR(_device, _swapchain, 1000000000,
                                 get_current_frame()._swapchainSemaphore,
                                 nullptr, &swapchainImageIndex));

  VkCommandBuffer cmd = get_current_frame()._mainCommandBuffer;
  VK_CHECK(vkResetCommandBuffer(
      cmd, 0));  // now that we know that the command
                 // buffer is done we can safely reset it
                 // this removes all commands and free its memory
  VkCommandBufferBeginInfo cmdBeginInfo = vkinit::command_buffer_begin_info(
      VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
  VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

  //  OLD
  //
  // // make the swapchain image into writable mode before rendering
  // vkutil::transition_image(cmd, _swapchainImages[swapchainImageIndex],
  //                          VK_IMAGE_LAYOUT_UNDEFINED,
  //                          VK_IMAGE_LAYOUT_UNDEFINED);
  //
  // // make a clear-color from frame number that will flash with 120 frame
  // period VkClearColorValue clearValue; float flash =
  // std::abs(std::sin(_frameNumber / 120.f));
  //
  // clearValue = {0.0f, 0.0f, flash, 1.0f};
  // VkImageSubresourceRange clearRange =
  //     vkinit::image_subresource_range(VK_IMAGE_ASPECT_COLOR_BIT);
  //
  // // clear image
  // vkCmdClearColorImage(cmd, _swapchainImages[swapchainImageIndex],
  //                      VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &clearRange);
  //
  // // make the swapchain image into presentable mode
  // vkutil::transition_image(cmd, _swapchainImages[swapchainImageIndex],
  //                          VK_IMAGE_LAYOUT_GENERAL,
  //                          VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
  //
  // // finaly the command buffer
  // VK_CHECK(vkEndCommandBuffer(cmd));

  // prepare the submission to the queue
  // we want to wait on the _presenSemapohre, as that semaphore is signaled when
  // the swapchain is ready we will signal the _renderSemapohre, to signal that
  // rendering has finished

  //// New implementation

  _drawExtent.width  = _drawImage.imageExtent.width;
  _drawExtent.height = _drawImage.imageExtent.height;

  VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

  // transition our main draw image into general layout so we can write into it
  // we will overwrite it all so we dont care about what was the older layout
  vkutil::transition_image(cmd, _drawImage.image, VK_IMAGE_LAYOUT_UNDEFINED,
                           VK_IMAGE_LAYOUT_GENERAL);

  draw_background(cmd);

  // transition the draw image and the swapchain image into their correct
  // transfer layouts
  vkutil::transition_image(cmd, _drawImage.image, VK_IMAGE_LAYOUT_GENERAL,
                           VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
  vkutil::transition_image(cmd, _swapchainImages[swapchainImageIndex],
                           VK_IMAGE_LAYOUT_UNDEFINED,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

  // execute a copy from the draw image into the swapchain
  vkutil::copy_image_to_image(cmd, _drawImage.image,
                              _swapchainImages[swapchainImageIndex],
                              _drawExtent, _swapchainExtent);

  // set swapchain image layout to Present so we can show it on the screen
  vkutil::transition_image(cmd, _swapchainImages[swapchainImageIndex],
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

  // finalize the command buffer (we can no longer add commands, but it can now
  // be executed)
  VK_CHECK(vkEndCommandBuffer(cmd));

  ///

  VkCommandBufferSubmitInfo cmdInfo = vkinit::command_buffer_submit_info(cmd);

  VkSemaphoreSubmitInfo waitInfo = vkinit::semaphore_submit_info(
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
      get_current_frame()._swapchainSemaphore);
  VkSemaphoreSubmitInfo signalInfo = vkinit::semaphore_submit_info(
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
      get_current_frame()._renderSemaphore);

  VkSubmitInfo2 submitInfo =
      vkinit::submit_info(&cmdInfo, &signalInfo, &waitInfo);

  // submit the command buffer to the queue and execute it.
  // _renderFence will now block until the graphic commands finish execution
  VK_CHECK(vkQueueSubmit2(_graphicsQueue, 1, &submitInfo,
                          get_current_frame()._renderFence));

  // prepare present
  //  this will put the image we just rendered to into the visible window.
  //  we want to wait on the _renderSemaphore for that,
  //  as its necessary that drawing commands have finished before the image is
  //  displayed to the user
  VkPresentInfoKHR presentInfo = {};
  presentInfo.sType            = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.pNext            = nullptr;
  presentInfo.pSwapchains      = &_swapchain;
  presentInfo.swapchainCount   = 1;

  presentInfo.pWaitSemaphores    = &get_current_frame()._renderSemaphore;
  presentInfo.waitSemaphoreCount = 1;

  presentInfo.pImageIndices = &swapchainImageIndex;

  VK_CHECK(vkQueuePresentKHR(_graphicsQueue, &presentInfo));

  // increase the number of frames drawn
  _frameNumber++;
}

void VulkanEngine::init_vulkan() {
  vkb::InstanceBuilder builder;
  // create the vulkan instance
  // abstrancts the creation of the VkInstance
  auto inst_ret = builder.set_app_name("Hello Triangle")
                      .request_validation_layers(bUseValidationLayers)
                      .use_default_debug_messenger()
                      .require_api_version(1, 3, 0)
                      .build();

  vkb::Instance vkb_inst = inst_ret.value();

  fmt::print("Vulkan instance created\n");
  // store the instance in the engine state
  _instance        = vkb_inst.instance;
  _debug_messenger = vkb_inst.debug_messenger;

  // create the surface
  SDL_Vulkan_CreateSurface(_window, _instance, &_surface);

  // vulkan 1.3 features
  VkPhysicalDeviceVulkan13Features features{
      .sType            = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
      .synchronization2 = true,
      .dynamicRendering = true,
  };

  fmt::println("Select a GPU");
  // use vkbootstrap to select a gpu.
  // We want a gpu that can write to the SDL surface and supports vulkan 1.3
  // with the correct features
  vkb::PhysicalDeviceSelector selector{vkb_inst};
  vkb::PhysicalDevice physicalDevice = selector.set_minimum_version(1, 3)
                                           .set_required_features_13(features)
                                           .set_surface(_surface)
                                           .select()
                                           .value();

  fmt::println("Selected GPU: {}", physicalDevice.name);
  fmt::println("Available extensions:");

  for (auto ext : physicalDevice.get_extensions()) {
    fmt::println("{}", ext);
  }

  // create the final vulkan device

  vkb::DeviceBuilder deviceBuilder{physicalDevice};

  vkb::Device vkbDevice = deviceBuilder.build().value();

  // Get the VkDevice handle used in the rest of a vulkan application
  _device    = vkbDevice.device;
  _chosenGPU = physicalDevice.physical_device;

  fmt::println("Creating graphics queue");
  // get a graphics queue from the device
  _graphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
  _graphicsQueueFamily =
      vkbDevice.get_queue_index(vkb::QueueType::graphics).value();

  fmt::println("Selected graphics queue: {}", _graphicsQueueFamily);

  // initialize the memory allocator
  fmt::print("Creating VMA Allocator\n");
  VmaAllocatorCreateInfo allocatorInfo = {};
  allocatorInfo.physicalDevice         = _chosenGPU;
  allocatorInfo.device                 = _device;
  allocatorInfo.instance               = _instance;
  allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
  vmaCreateAllocator(&allocatorInfo, &_allocator);

  _mainDeletionQueue.push([&]() { vmaDestroyAllocator(_allocator); });
}

void VulkanEngine::create_swapchain(uint32_t width, uint32_t height) {
  // create the swapchain builder and set the format
  vkb::SwapchainBuilder swapchainBuilder{_chosenGPU, _device, _surface};
  _swapchainImageFormat = VK_FORMAT_B8G8R8A8_UNORM;

  vkb::Swapchain vkbSwapchain =
      swapchainBuilder
          .set_desired_format(VkSurfaceFormatKHR{
              .format     = _swapchainImageFormat,
              .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR})
          .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
          .set_desired_extent(width, height)
          .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
          .build()
          .value();

  _swapchainExtent      = vkbSwapchain.extent;
  _swapchain            = vkbSwapchain.swapchain;
  _swapchainImages      = vkbSwapchain.get_images().value();
  _swapchainImageFormat = vkbSwapchain.image_format;
}
void VulkanEngine::destroy_swapchain() {
  vkDestroySwapchainKHR(_device, _swapchain, nullptr);
  // destroy swapchain resources
  for (int i = 0; i < _swapchainImageViews.size(); i++) {
    vkDestroyImageView(_device, _swapchainImageViews[i], nullptr);
  }
}
void VulkanEngine::init_swapchain() {
  create_swapchain(_windowExtent.width, _windowExtent.height);
  // draw image size will match the window
  VkExtent3D drawImageExtent = {_windowExtent.width, _windowExtent.height, 1};

  // hardcoding the draw format to 32 bit float
  _drawImage.imageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
  _drawImage.imageExtent = drawImageExtent;

  VkImageUsageFlags drawImageUsages{};
  drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
  drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
  drawImageUsages |= VK_IMAGE_USAGE_STORAGE_BIT;
  drawImageUsages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  VkImageCreateInfo rimg_info = vkinit::image_create_info(
      _drawImage.imageFormat, drawImageUsages, drawImageExtent);

  // for the draw image, we want to allocate it from gpu local memory
  VmaAllocationCreateInfo rimg_allocinfo = {};
  rimg_allocinfo.usage                   = VMA_MEMORY_USAGE_GPU_ONLY;
  rimg_allocinfo.requiredFlags =
      VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  // allocate and create the image
  vmaCreateImage(_allocator, &rimg_info, &rimg_allocinfo, &_drawImage.image,
                 &_drawImage.allocation, nullptr);

  // build a image-view for the draw image to use for rendering
  VkImageViewCreateInfo rview_info = vkinit::imageview_create_info(
      _drawImage.imageFormat, _drawImage.image, VK_IMAGE_ASPECT_COLOR_BIT);

  VK_CHECK(
      vkCreateImageView(_device, &rview_info, nullptr, &_drawImage.imageView));

  // add to deletion queues
  _mainDeletionQueue.push([=, this]() {
    vkDestroyImageView(_device, _drawImage.imageView, nullptr);
    vmaDestroyImage(_allocator, _drawImage.image, _drawImage.allocation);
  });
}
void VulkanEngine::init_commands() {
  // create a command pool for commands submitted to the graphics queue.
  // we also want the pool to allow for resetting of individual command buffers
  fmt::print("Creating command pool\n");

  VkCommandPoolCreateInfo commandPoolInfo = vkinit::command_pool_create_info(
      _graphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

  for (int i = 0; i < FRAME_OVERLAP; i++) {
    VK_CHECK(vkCreateCommandPool(_device, &commandPoolInfo, nullptr,
                                 &_frames[i]._commandPool));

    fmt::print("Allocating command buffer {}\n", i);
    // allocate the default command buffer that we will use for rendering
    VkCommandBufferAllocateInfo cmdAllocInfo =
        vkinit::command_buffer_allocate_info(_frames[i]._commandPool, 1);
    VK_CHECK(vkAllocateCommandBuffers(_device, &cmdAllocInfo,
                                      &_frames[i]._mainCommandBuffer));
  }
}
void VulkanEngine::init_sync_structures() {
  // create synchronization structures
  VkSemaphoreCreateInfo semaphoreCreateInfo =
      vkinit::semaphore_create_info(VK_SEMAPHORE_TYPE_BINARY);  // not sure
  VkFenceCreateInfo fenceCreateInfo = vkinit::fence_create_info(
      VK_FENCE_CREATE_SIGNALED_BIT);  // we want to create the fence with the
                                      // signaled bit set, so we don't wait on
                                      // the first render of each frame
  for (int i = 0; i < FRAME_OVERLAP; i++) {
    VK_CHECK(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr,
                               &_frames[i]._swapchainSemaphore));
    VK_CHECK(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr,
                               &_frames[i]._renderSemaphore));
    VK_CHECK(vkCreateFence(_device, &fenceCreateInfo, nullptr,
                           &_frames[i]._renderFence));
  }
}

void VulkanEngine::draw_background(VkCommandBuffer cmd) {
  VkClearColorValue clearValue;
  float flash = std::abs(std::sin(_frameNumber / 20.f));
  clearValue  = {{0.0f, 0.0f, flash, 1.0f}};
  VkImageSubresourceRange clearrange =
      vkinit::image_subresource_range(VK_IMAGE_ASPECT_COLOR_BIT);
  vkCmdClearColorImage(cmd, _swapchainImages[0], VK_IMAGE_LAYOUT_GENERAL,
                       &clearValue, 1, &clearrange);

  // clear the image
  vkCmdClearColorImage(cmd, _drawImage.image, VK_IMAGE_LAYOUT_GENERAL,
                       &clearValue, 1, &clearrange);
}

void DeletionQueue::push(std::function<void()>&& function) {
  deletors.push_back(
      function);  // stores a lambda assigned to a callback function
}
void DeletionQueue::flush() {
  // interante in backorder, falling the functions
  for (auto f = deletors.rbegin(); f != deletors.rend(); f++) {
    (*f)();
  }
  deletors.clear();
}
