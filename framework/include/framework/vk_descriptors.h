#include <vulkan/vulkan_core.h>

#include <vector>

struct DescriptorLayoutBuilder {
  std::vector<VkDescriptorSetLayoutBinding> bindings;

  void add_binding(uint32_t binding, VkDescriptorType type);
  void clear();
  void extracted(VkDevice &device, VkDescriptorSetLayoutCreateInfo &info,
                 VkDescriptorSetLayout &set);
  VkDescriptorSetLayout build(VkDevice device, VkShaderStageFlags shaderStages,
                              void *pNext                            = nullptr,
                              VkDescriptorSetLayoutCreateFlags flags = 0);
};

struct DescriptorAllocator {
  struct PoolSizeRatio {
    VkDescriptorType type;
    float ratio;
  };

  // Descriptor allocation happens through VkDescriptorPool. These are object
  // that need to be pre-initialized with some size and types of descriptors for
  // it. It's possible to have 1 very big descriptor pool tha thandles the
  // entire engine but requires to know what descriptors we will be using ahead
  // of time. To scale it better we will have multiple descriptor pools for
  // different parts of the project.
  VkDescriptorPool pool;

  void init_pool(VkDevice device, uint32_t maxSets,
                 std::vector<PoolSizeRatio> poolRatios);
  void clear_desciptors(VkDevice device);
  void destroy_pool(VkDevice device);

  VkDescriptorSet allocate(VkDevice device, VkDescriptorSetLayout layout);
};
