#include <SDL.h>
#include <SDL_vulkan.h>
#include <fmt/base.h>
#include <fmt/core.h>
#include <framework/vk_engine.h>
#include <framework/vk_image.h>
#include <framework/vk_initializers.h>
#include <framework/vk_pipelines.h>
#include <framework/vk_types.h>
#include <vulkan/vulkan_core.h>

#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <thread>

#include "SDL_events.h"

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

constexpr bool bUseValidationLayers = true;

VulkanEngine* loadedEngine = nullptr;

VulkanEngine& VulkanEngine::Get() { return *loadedEngine; }

void VulkanEngine::init(bool bEnableValidationLayers) {
  // only one engine initialization is allowed with the application.
  assert(loadedEngine == nullptr);
  loadedEngine = this;

  // We initialize SDL and create a window with it.
  init_vulkan();
  init_commands();
  init_sync_structures();
  init_descriptors();
  init_pipelines();

  _isInitialized = true;
}
void VulkanEngine::cleanup() {
  if (_isInitialized) {
    // changing the order of distructions can cause errors
    fmt::print("Cleaning up Vulkan Engine\n");
    vkDeviceWaitIdle(_device);

    vkDestroyCommandPool(_device, _commandPool, nullptr);

    // destroy synchronization objects
    vkDestroyFence(_device, _renderFence, nullptr);
    vkDestroySemaphore(_device, _renderSemaphore, nullptr);
    vkDestroySemaphore(_device, _swapchainSemaphore, nullptr);

    _mainDeletionQueue.flush();

    vkDestroyDevice(_device, nullptr);
    vkDestroySurfaceKHR(_instance, _surface, nullptr);

    vkb::destroy_debug_utils_messenger(_instance, _debug_messenger);
    vkDestroyInstance(_instance, nullptr);

    loadedEngine = nullptr;
  }
}

void VulkanEngine::compute() { fmt::print("main compute\n"); }

void VulkanEngine::run_compute(const std::vector<float>& initial_conditions,
                               uint32_t timesteps) {
  fmt::print("Starting Compute Configuration\n");

  void* data;
  // vkMapMemory(_device, inputBufferMemory->GetMemory(), 0,
  // initial_conditions.size() * sizeof(float), 0, &data); memcpy(data,
  // initial_conditions.data(), initial_conditions.size() * sizeof(float));
  // vkUnmapMemory(_device, inputBufferMemory->GetMemory());

  VkBufferCreateInfo bufCreateInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
  bufCreateInfo.size               = initial_conditions.size() * sizeof(float);
  bufCreateInfo.usage              = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

  VmaAllocationCreateInfo allocCreateInfo = {};
  allocCreateInfo.usage                   = VMA_MEMORY_USAGE_AUTO;
  allocCreateInfo.flags =
      VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

  vmaCreateBuffer(_allocator, &bufCreateInfo, &allocCreateInfo, &_inputBuffer,
                  &_inputBufferMemory, nullptr);

  // Record command buffer
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  PushConstants pushConstants{0.1, 0.2, 0.03, 4};

  for (uint32_t i = 0; i < timesteps; i++) {
    vkBeginCommandBuffer(_mainCommandBuffer, &beginInfo);

    vkCmdBindPipeline(_mainCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE,
                      _computePipeline);
    vkCmdBindDescriptorSets(_mainCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE,
                            pipelineLayout, 0, 1, &_drawImageDescriptors, 0,
                            nullptr);

    vkCmdPushConstants(_mainCommandBuffer, pipelineLayout,
                       VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(PushConstants),
                       &pushConstants);

    // Dispatch compute shader
    // vkCmdDispatch(_mainCommandBuffer, (gridSize + 255) / 256, 1, 1);

    // Add memory barrier for buffer synchronization
    VkMemoryBarrier barrier{};
    barrier.sType         = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
    barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(_mainCommandBuffer,
                         VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                         VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0, 1, &barrier,
                         0, nullptr, 0, nullptr);

    // Swap buffers for next iteration
    std::swap(_inputBuffer, _outputBuffer);

    vkEndCommandBuffer(_mainCommandBuffer);

    // Submit command buffer
    VkSubmitInfo submitInfo{};
    submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &_mainCommandBuffer;

    vkQueueSubmit(_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(_graphicsQueue);
  }

  compute();
}

void VulkanEngine::init_vulkan() {
  vkb::InstanceBuilder builder;
  // create the vulkan instance
  // abstrancts the creation of the VkInstance
  auto inst_ret = builder.set_app_name("Hello Triangle")
                      .request_validation_layers(bUseValidationLayers)
                      .enable_extension(VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME)
                      .use_default_debug_messenger()
                      .require_api_version(1, 3, 0)
                      .build();

  vkb::Instance vkb_inst = inst_ret.value();

  fmt::print("Vulkan instance created\n");
  // store the instance in the engine state
  _instance        = vkb_inst.instance;
  _debug_messenger = vkb_inst.debug_messenger;

  // vulkan 1.3 features
  VkPhysicalDeviceVulkan13Features features13{
      .sType            = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
      .synchronization2 = true,
      .dynamicRendering = false,
  };

  fmt::println("Select a GPU");
  // use vkbootstrap to select a gpu.
  // We want a gpu that can write to the SDL surface and supports vulkan 1.3
  // with the correct features
  vkb::PhysicalDeviceSelector selector{vkb_inst};
  // vkb::PhysicalDevice physicalDevice =
  //     selector
  //         .set_minimum_version(1, 3)
  //         //.set_required_features_13(features)
  //         .select_first_device_unconditionally()
  //         .select()
  //         .value();

  VkHeadlessSurfaceCreateInfoEXT surfaceCreateInfo{};
  surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_HEADLESS_SURFACE_CREATE_INFO_EXT;
  surfaceCreateInfo.pNext = nullptr;
  surfaceCreateInfo.flags = 0;

  vkCreateHeadlessSurfaceEXT(_instance, &surfaceCreateInfo, nullptr, &_surface);

  VkPhysicalDeviceFeatures features{};
  features.shaderFloat64 = VK_TRUE;
  auto physicalDevice =
      selector
          .set_minimum_version(1, 2)  // Vulkan 1.2 or higher
          .prefer_gpu_device_type()   // Prefer discrete GPUs
          .add_required_extensions({VK_KHR_SHADER_FLOAT16_INT8_EXTENSION_NAME,
                                    VK_KHR_16BIT_STORAGE_EXTENSION_NAME})
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
  allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT ||
                        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
  vmaCreateAllocator(&allocatorInfo, &_allocator);

  _mainDeletionQueue.push([&]() { vmaDestroyAllocator(_allocator); });
}

void VulkanEngine::init_commands() {
  // create a command pool for commands submitted to the graphics queue.
  // we also want the pool to allow for resetting of individual command buffers
  fmt::print("Creating command pool\n");

  VkCommandPoolCreateInfo commandPoolInfo = vkinit::command_pool_create_info(
      _graphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

  VK_CHECK(
      vkCreateCommandPool(_device, &commandPoolInfo, nullptr, &_commandPool));

  fmt::print("Allocating command buffer \n");
  // allocate the default command buffer that we will use for rendering
  VkCommandBufferAllocateInfo cmdAllocInfo =
      vkinit::command_buffer_allocate_info(_commandPool, 1);
  VK_CHECK(
      vkAllocateCommandBuffers(_device, &cmdAllocInfo, &_mainCommandBuffer));
}
void VulkanEngine::init_sync_structures() {
  // create synchronization structures
  VkSemaphoreCreateInfo semaphoreCreateInfo =
      vkinit::semaphore_create_info(VK_SEMAPHORE_TYPE_BINARY);  // not sure
  VkFenceCreateInfo fenceCreateInfo = vkinit::fence_create_info(
      VK_FENCE_CREATE_SIGNALED_BIT);  // we want to create the fence with the
                                      // signaled bit set, so we don't wait on
                                      // the first render of each frame
  VK_CHECK(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr,
                             &_swapchainSemaphore));
  VK_CHECK(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr,
                             &_renderSemaphore));
  VK_CHECK(vkCreateFence(_device, &fenceCreateInfo, nullptr, &_renderFence));
}

void VulkanEngine::init_descriptors() {
  // create a descriptor pool that will hold 10 sets with 1 image each
  // the storage type is used for a image that can be written to from a compute
  // shader
  std::vector<DescriptorAllocator::PoolSizeRatio> sizes = {
      {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1}};

  globalDescriptorAllocator.init_pool(_device, 10, sizes);

  // make the descriptor set layout for out compute draw
  {
    DescriptorLayoutBuilder builder;
    builder.add_binding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
    builder.add_binding(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
    _computeDescriptorLayout =
        builder.build(_device, VK_SHADER_STAGE_COMPUTE_BIT);
  }
  // using the builder to make a layout with only 1 single binding at binding
  // number 0, of type matching the pool We now allocate one of them, and make
  // it so it points to the draw image.

  // allocate a descriptor set for our draw image
  _drawImageDescriptors =
      globalDescriptorAllocator.allocate(_device, _computeDescriptorLayout);

  VkDescriptorBufferInfo bufInfo{};

  // make sure both the descriptor allocator and the new layout get cleaned up
  // properly
  _mainDeletionQueue.push([&]() {
    globalDescriptorAllocator.destroy_pool(_device);

    vkDestroyDescriptorSetLayout(_device, _computeDescriptorLayout, nullptr);
  });
}

void VulkanEngine::init_pipelines() { init_background_pipelines(); };

void VulkanEngine::init_background_pipelines() {
  fmt::print("Creating compute pipeline layout\n");
  VkPushConstantRange pushConstantRange{};
  pushConstantRange.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
  pushConstantRange.offset     = 0;
  pushConstantRange.size       = sizeof(_pushConstants);

  VkPipelineLayoutCreateInfo pipelineLayoutCI{};
  pipelineLayoutCI.sType       = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutCI.pNext       = nullptr;
  pipelineLayoutCI.pSetLayouts = &_computeDescriptorLayout;
  pipelineLayoutCI.setLayoutCount = 1;
  //  pipelineLayoutCI.flags                  = VK_SHADER_STAGE_COMPUTE_BIT;
  pipelineLayoutCI.pPushConstantRanges    = &pushConstantRange;
  pipelineLayoutCI.pushConstantRangeCount = 1;

  VK_CHECK(vkCreatePipelineLayout(_device, &pipelineLayoutCI, nullptr,
                                  &pipelineLayout));

  fmt::print("Loading compute shader\n");

  VkShaderModule shaderModule;
  if (!vkutil::load_shader_module("../../../shaders/heat_shader.spv", _device,
                                  &shaderModule)) {
    fmt::print("Error when building the compute shader \n");
  }

  VkPipelineShaderStageCreateInfo shaderStageCI{};
  shaderStageCI.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shaderStageCI.pNext  = nullptr;
  shaderStageCI.stage  = VK_SHADER_STAGE_COMPUTE_BIT;
  shaderStageCI.module = shaderModule;
  shaderStageCI.pName  = "main";

  VkComputePipelineCreateInfo computePipelineCI{};
  computePipelineCI.sType  = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
  computePipelineCI.pNext  = nullptr;
  computePipelineCI.layout = pipelineLayout;
  computePipelineCI.stage  = shaderStageCI;

  fmt::print("Creating compute pipeline\n");
  VK_CHECK(vkCreateComputePipelines(_device, VK_NULL_HANDLE, 1,
                                    &computePipelineCI, nullptr,
                                    &_computePipeline));

  vkDestroyShaderModule(_device, shaderModule, nullptr);

  _mainDeletionQueue.push([&]() {
    vkDestroyPipelineLayout(_device, pipelineLayout, nullptr);
    vkDestroyPipeline(_device, _computePipeline, nullptr);
  });
}

void VulkanEngine::init_buffers() {
  // create the input buffer
  VkBufferCreateInfo bufferInfo = vkinit::buffer_create_info(
      VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
      sizeof(float) * _gridSize);

  VmaAllocationCreateInfo vmaAllocCI = {};
  vmaAllocCI.usage                   = VMA_MEMORY_USAGE_CPU_TO_GPU;
  VmaAllocationInfo vmaallocInputInfo{};
  VK_CHECK(vmaCreateBuffer(_allocator, &bufferInfo, &vmaAllocCI, &_inputBuffer,
                           &_inputBufferMemory, &vmaallocInputInfo));
  // create the output buffer
  vmaAllocCI.usage = VMA_MEMORY_USAGE_GPU_TO_CPU;
  bufferInfo       = vkinit::buffer_create_info(
      VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
      sizeof(float) * _gridSize);
  VmaAllocationInfo vmaallocOutputInfo{};
  VK_CHECK(vmaCreateBuffer(_allocator, &bufferInfo, &vmaAllocCI, &_outputBuffer,
                           &_outputBufferMemory, &vmaallocOutputInfo));

  // add the buffers to the deletion queue
  _mainDeletionQueue.push([&]() {
    vmaDestroyBuffer(_allocator, _inputBuffer, _inputBufferMemory);
    vmaDestroyBuffer(_allocator, _outputBuffer, _outputBufferMemory);
  });
}
void VulkanEngine::set_costants(float dt, float dx, float alpha,
                                uint32_t size) {
  _pushConstants.dt    = dt;
  _pushConstants.dx    = dx;
  _pushConstants.alpha = alpha;
  _pushConstants.size  = size;
}

void DeletionQueue::push(std::function<void()>&& function) {
  deletors.push_back(
      function);  // stores a lambda assigned to a callback function
}
void DeletionQueue::flush() {
  // interante in backorder, falling the functions
  for (auto f = deletors.rbegin(); f != deletors.rend(); f++) {
    fmt::print("Deleting from queue\n");
    (*f)();
  }
  deletors.clear();
}
