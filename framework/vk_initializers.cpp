#include <framework/vk_initializers.h>

VkCommandPoolCreateInfo vkinit::command_pool_create_info(
    uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags /*= 0*/) {
  VkCommandPoolCreateInfo info = {};
  info.sType                   = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  info.pNext                   = nullptr;
  info.queueFamilyIndex        = queueFamilyIndex;
  info.flags                   = flags;
  return info;
}

VkCommandBufferAllocateInfo vkinit::command_buffer_allocate_info(
    VkCommandPool pool, uint32_t count /*= 1*/) {
  VkCommandBufferAllocateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  info.pNext = nullptr;

  info.commandPool        = pool;
  info.commandBufferCount = count;
  info.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  return info;
}

VkFenceCreateInfo vkinit::fence_create_info(VkFenceCreateFlags flags /*= 0*/) {
  VkFenceCreateInfo info = {};
  info.sType             = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  info.pNext             = nullptr;
  info.flags             = flags;
  return info;
}

VkSemaphoreCreateInfo vkinit::semaphore_create_info(
    VkSemaphoreCreateFlags flags) {
  VkSemaphoreCreateInfo info = {};
  info.sType                 = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  info.pNext                 = nullptr;
  info.flags                 = 0;
  return info;
}
VkCommandBufferBeginInfo vkinit::command_buffer_begin_info(
    VkCommandBufferUsageFlags flags) {
  VkCommandBufferBeginInfo info = {};
  info.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  info.pNext                    = nullptr;
  info.flags                    = flags;
  info.pInheritanceInfo         = nullptr;
  return info;
}
VkImageSubresourceRange vkinit::image_subresource_range(
    VkImageAspectFlags aspectMask) {
  VkImageSubresourceRange range = {};
  range.aspectMask              = aspectMask;
  range.baseMipLevel            = 0;
  range.levelCount              = VK_REMAINING_MIP_LEVELS;
  range.baseArrayLayer          = 0;
  range.layerCount              = VK_REMAINING_ARRAY_LAYERS;

  return range;
}
VkSemaphoreSubmitInfo vkinit::semaphore_submit_info(
    VkPipelineStageFlags2 stageMask, VkSemaphore semaphore) {
  VkSemaphoreSubmitInfo submitInfo{};
  submitInfo.sType       = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
  submitInfo.pNext       = nullptr;
  submitInfo.semaphore   = semaphore;
  submitInfo.stageMask   = stageMask;
  submitInfo.deviceIndex = 0;
  submitInfo.value       = 1;

  return submitInfo;
}

VkCommandBufferSubmitInfo vkinit::command_buffer_submit_info(
    VkCommandBuffer cmd) {
  VkCommandBufferSubmitInfo info{};
  info.sType         = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
  info.pNext         = nullptr;
  info.commandBuffer = cmd;
  info.deviceMask    = 0;

  return info;
}

VkSubmitInfo2 vkinit::submit_info(VkCommandBufferSubmitInfo* cmd,
                                  VkSemaphoreSubmitInfo* signalSemaphoreInfo,
                                  VkSemaphoreSubmitInfo* waitSemaphoreInfo) {
  VkSubmitInfo2 info = {};
  info.sType         = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
  info.pNext         = nullptr;

  info.waitSemaphoreInfoCount = waitSemaphoreInfo == nullptr ? 0 : 1;
  info.pWaitSemaphoreInfos    = waitSemaphoreInfo;

  info.signalSemaphoreInfoCount = signalSemaphoreInfo == nullptr ? 0 : 1;
  info.pSignalSemaphoreInfos    = signalSemaphoreInfo;

  info.commandBufferInfoCount = 1;
  info.pCommandBufferInfos    = cmd;

  return info;
}
VkImageCreateInfo vkinit::image_create_info(VkFormat format,
                                            VkImageUsageFlags usageFlags,
                                            VkExtent3D extent) {
  VkImageCreateInfo info = {};
  info.sType             = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  info.pNext             = nullptr;

  info.imageType = VK_IMAGE_TYPE_2D;

  info.format = format;
  info.extent = extent;

  info.mipLevels   = 1;
  info.arrayLayers = 1;

  // for MSAA. we will not be using it by default, so default it to 1 sample per
  // pixel.
  info.samples = VK_SAMPLE_COUNT_1_BIT;

  // optimal tiling, which means the image is stored on the best gpu format
  info.tiling = VK_IMAGE_TILING_OPTIMAL;
  info.usage  = usageFlags;

  return info;
}

VkImageViewCreateInfo vkinit::imageview_create_info(
    VkFormat format, VkImage image, VkImageAspectFlags aspectFlags) {
  // build a image-view for the depth image to use for rendering
  VkImageViewCreateInfo info = {};
  info.sType                 = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  info.pNext                 = nullptr;

  info.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
  info.image                           = image;
  info.format                          = format;
  info.subresourceRange.baseMipLevel   = 0;
  info.subresourceRange.levelCount     = 1;
  info.subresourceRange.baseArrayLayer = 0;
  info.subresourceRange.layerCount     = 1;
  info.subresourceRange.aspectMask     = aspectFlags;

  return info;
}
VkBufferCreateInfo vkinit::buffer_create_info(VkBufferUsageFlags flags,
                                              VkDeviceSize size) {
  VkBufferCreateInfo info = {};
  info.sType              = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  info.pNext              = nullptr;
  info.size               = size;
  info.usage              = flags;
  return info;
}
