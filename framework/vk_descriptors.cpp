#include <fmt/base.h>
#include <framework/vk_descriptors.h>
#include <framework/vk_types.h>
#include <vulkan/vulkan_core.h>

// With the example of the compute shader, that's binding=0, and type is
// VK_DESCRIPTOR_TYPE_STORAGE_IMAGE which is a writable image (for project use
// SHOULD be BUFFER, but must check)
void DescriptorLayoutBuilder::add_binding(uint32_t binding,
                                          VkDescriptorType type) {
  VkDescriptorSetLayoutBinding newbind{};
  newbind.binding         = binding;
  newbind.descriptorCount = 1;
  newbind.descriptorType  = type;

  bindings.push_back(newbind);
}

void DescriptorLayoutBuilder::clear() { bindings.clear(); }

VkDescriptorSetLayout DescriptorLayoutBuilder::build(
    VkDevice device, VkShaderStageFlags shaderStages, void* pNext,
    VkDescriptorSetLayoutCreateFlags flags) {
  // loop though the bindings and add the stage.
  for (auto& b : bindings) {
    b.stageFlags |= shaderStages;
  }

  VkDescriptorSetLayoutCreateInfo info = {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
  info.pNext = pNext;

  info.pBindings    = bindings.data();
  info.bindingCount = (uint32_t)bindings.size();
  info.flags        = flags;

  VkDescriptorSetLayout set;
  VK_CHECK(vkCreateDescriptorSetLayout(device, &info, nullptr, &set));

  return set;
}

void DescriptorAllocator::init_pool(VkDevice device, uint32_t maxSets,
                                    std::vector<PoolSizeRatio> poolRatios) {
  std::vector<VkDescriptorPoolSize> poolSizes;

  // PoolSizeRatio is a structure that contains a type of descriptor (the same
  // VkDescriptorType as on the binding above) alongside a ratio to multiply the
  // maxSets parameter, This allows to control how bit the pool is, maxSets
  // controls how may VkDescriptorsets we can create from the pool in total and
  // the pool size gives how many individual bindings of a given type are owned.
  for (PoolSizeRatio ratio : poolRatios) {
    poolSizes.push_back(VkDescriptorPoolSize{
        .type            = ratio.type,
        .descriptorCount = uint32_t(ratio.ratio * maxSets)});
  }

  VkDescriptorPoolCreateInfo pool_info = {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
  pool_info.flags         = 0;
  pool_info.maxSets       = maxSets;
  pool_info.poolSizeCount = (uint32_t)poolSizes.size();
  pool_info.pPoolSizes    = poolSizes.data();

  vkCreateDescriptorPool(device, &pool_info, nullptr, &pool);
}

void DescriptorAllocator::clear_desciptors(VkDevice device) {
  // this destroys all the descriptors created from the pool and put it back to
  // initial state, but doesn't destroy the pool iteself.
  vkResetDescriptorPool(device, pool, 0);
}

void DescriptorAllocator::destroy_pool(VkDevice device) {
  vkDestroyDescriptorPool(device, pool, nullptr);
}
VkDescriptorSet DescriptorAllocator::allocate(VkDevice device,
                                              VkDescriptorSetLayout layout) {
  VkDescriptorSetAllocateInfo allocInfo = {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
  allocInfo.pNext              = nullptr;
  allocInfo.descriptorPool     = pool;
  allocInfo.descriptorSetCount = 1;
  allocInfo.pSetLayouts        = &layout;

  VkDescriptorSet ds;
  VK_CHECK(vkAllocateDescriptorSets(device, &allocInfo, &ds));

  return ds;
}

// ARRIVATO A INITIALIZING THE LAYOUT AND DESCIPTORS
