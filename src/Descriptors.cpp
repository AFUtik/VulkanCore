#include "Descriptors.hpp"

// std
#include <cassert>
#include <stdexcept>
#include <iostream>
 
namespace myvk {
 
// *************** Descriptor Set Layout Builder *********************
 
DescriptorSetLayout::Builder &DescriptorSetLayout::Builder::addBinding(
    uint32_t binding,
    VkDescriptorType descriptorType,
    VkShaderStageFlags stageFlags,
    uint32_t count) {
  assert(bindings.count(binding) == 0 && "Binding already in use");
  VkDescriptorSetLayoutBinding layoutBinding{};
  layoutBinding.binding = binding;
  layoutBinding.descriptorType = descriptorType;
  layoutBinding.descriptorCount = count;
  layoutBinding.stageFlags = stageFlags;
  bindings[binding] = layoutBinding;
  return *this;
}
 
std::unique_ptr<DescriptorSetLayout> DescriptorSetLayout::Builder::build() const {
  return std::make_unique<DescriptorSetLayout>(device, bindings);
}
 
// *************** Descriptor Set Layout *********************
 
DescriptorSetLayout::DescriptorSetLayout(
    Device &device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
    : device{device}, bindings{bindings} {
  std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
  for (auto kv : bindings) {
    setLayoutBindings.push_back(kv.second);
  }
 
  VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
  descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
  descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();
 
  if (vkCreateDescriptorSetLayout(
          device.device(),
          &descriptorSetLayoutInfo,
          nullptr,
          &descriptorSetLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor set layout!");
  }
}
 
DescriptorSetLayout::~DescriptorSetLayout() {
  if (descriptorSetLayout == VK_NULL_HANDLE)
        return;

  VkDevice dev = device.device();
  VkDescriptorSetLayout layout = descriptorSetLayout;

  device.getDeletionQueue().push_function(
      [dev, layout]() {
          vkDestroyDescriptorSetLayout(dev, layout, nullptr);
      }
  );
  
  descriptorSetLayout = VK_NULL_HANDLE;
}
 
// *************** Descriptor Pool Builder *********************
 
DescriptorPoolManager::Builder &DescriptorPoolManager::Builder::addPoolSize(
    VkDescriptorType descriptorType, uint32_t count) {
  poolSizes.push_back({descriptorType, count});
  return *this;
}
 
DescriptorPoolManager::Builder &DescriptorPoolManager::Builder::setPoolFlags(
    VkDescriptorPoolCreateFlags flags) {
  poolFlags = flags;
  return *this;
}
DescriptorPoolManager::Builder &DescriptorPoolManager::Builder::setMaxSets(uint32_t count) {
  maxSets = count;
  return *this;
}
 
std::unique_ptr<DescriptorPoolManager> DescriptorPoolManager::Builder::build() const {
  return std::make_unique<DescriptorPoolManager>(device, maxSets, poolFlags, poolSizes);
}
 
// *************** Descriptor Pool *********************
 
DescriptorPoolManager::DescriptorPoolManager(
    Device &device,
    uint32_t maxSets,
    VkDescriptorPoolCreateFlags poolFlags,
    const std::vector<VkDescriptorPoolSize> &poolSizes) : device{device}, poolFlags{poolFlags}, maxSets(maxSets)
{
    this->poolSizes = poolSizes;
    allocateNewPool();
}
 
DescriptorPoolManager::~DescriptorPoolManager() {
  VkDevice dev = device.device();
  std::vector<VkDescriptorPool> pools = descriptorPools;

  device.getDeletionQueue().push_function(
      [dev, pools]() {
          for(VkDescriptorPool pool : pools) {
            if(pool == VK_NULL_HANDLE) continue;
            vkDestroyDescriptorPool(dev, pool, nullptr);
          }
      }
  );
}

void DescriptorPoolManager::allocateNewPool() {
    VkDescriptorPoolCreateInfo descriptorPoolInfo{};
    descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    descriptorPoolInfo.pPoolSizes = poolSizes.data();
    descriptorPoolInfo.maxSets = maxSets;
    descriptorPoolInfo.flags = poolFlags;

    VkDescriptorPool newPool;
    if (vkCreateDescriptorPool(device.device(), &descriptorPoolInfo, nullptr, &newPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }

    freePool = newPool;
    descriptorPools.push_back(newPool);

    maxSets*=2;
    for(VkDescriptorPoolSize& size : poolSizes) {
        size.descriptorCount*=2;
    }
}

bool DescriptorPoolManager::allocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout, DescriptorSetData &descriptor) {
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = freePool;
    allocInfo.pSetLayouts = &descriptorSetLayout;
    allocInfo.descriptorSetCount = 1;
    
    VkResult success = vkAllocateDescriptorSets(device.device(), &allocInfo, &descriptor.set);

    if (success == VK_ERROR_OUT_OF_POOL_MEMORY) {
        allocateNewPool();
        allocInfo.descriptorPool = freePool;
        success = vkAllocateDescriptorSets(device.device(), &allocInfo, &descriptor.set);

        std::cout << "New Pool has been allocated." << std::endl;
    } 
    if(success != VK_SUCCESS) return false;

    descriptor.pool = freePool;
    descriptor.layout = descriptorSetLayout;

    return true;
}
 
void DescriptorPoolManager::freeDescriptors(std::vector<DescriptorSetData> &descriptors) const {
    /*
      vkFreeDescriptorSets(
      device.device(),
      freePool,
      static_cast<uint32_t>(descriptors.size()),
      descriptors.data());
    */
}
 
void DescriptorPoolManager::resetPool() {
  vkResetDescriptorPool(device.device(), freePool, 0);
}
 
// *************** Descriptor Writer *********************
 
DescriptorWriter::DescriptorWriter(DescriptorSetLayout &setLayout, DescriptorPoolManager &poolManager) : setLayout{setLayout}, poolManager{poolManager} {}
 
DescriptorWriter &DescriptorWriter::writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo) {
  assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");
 
  auto &bindingDescription = setLayout.bindings[binding];
 
  assert(
      bindingDescription.descriptorCount == 1 &&
      "Binding single descriptor info, but binding expects multiple");
 
  VkWriteDescriptorSet write{};
  write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  write.descriptorType = bindingDescription.descriptorType;
  write.dstBinding = binding;
  write.pBufferInfo = bufferInfo;
  write.descriptorCount = 1;
 
  writes.push_back(write);
  return *this;
}
 
DescriptorWriter &DescriptorWriter::writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo) {
  assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");
 
  auto &bindingDescription = setLayout.bindings[binding];
 
  assert(
      bindingDescription.descriptorCount == 1 &&
      "Binding single descriptor info, but binding expects multiple");
 
  VkWriteDescriptorSet write{};
  write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  write.descriptorType = bindingDescription.descriptorType;
  write.dstBinding = binding;
  write.pImageInfo = imageInfo;
  write.descriptorCount = 1;
 
  writes.push_back(write);
  return *this;
}
 
bool DescriptorWriter::build(DescriptorSetData &set) {
  bool success = poolManager.allocateDescriptor(setLayout.getDescriptorSetLayout(), set);
  if (!success) {
    return false;
  }
  overwrite(set);
  return true;
}
 
void DescriptorWriter::overwrite(DescriptorSetData &set) {
  for (auto &write : writes) {
    write.dstSet = set.set;
  }
  vkUpdateDescriptorSets(poolManager.device.device(), writes.size(), writes.data(), 0, nullptr);
}
 
}