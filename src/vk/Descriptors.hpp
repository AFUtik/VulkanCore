#pragma once
 
#include "Device.hpp"
 
// std
#include <memory>
#include <unordered_map>
#include <vector>
 
namespace myvk {
 
class DescriptorSetLayout {
public:
    class Builder {
    public:
        Builder(Device &device) : device{device} {}
    
        Builder &addBinding(
            uint32_t binding,
            VkDescriptorType descriptorType,
            VkShaderStageFlags stageFlags,
            uint32_t count = 1);
        std::unique_ptr<DescriptorSetLayout> build() const;
    
    private:
        Device &device;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
    };
 
    DescriptorSetLayout(Device &device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
    ~DescriptorSetLayout();

    DescriptorSetLayout(const DescriptorSetLayout &) = delete;
    DescriptorSetLayout &operator=(const DescriptorSetLayout &) = delete;
    
    VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }
    
private:
    Device &device;
    VkDescriptorSetLayout descriptorSetLayout;
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;
    
    friend class Device;
    friend class DescriptorWriter;
};

class DescriptorPoolManager {
public:
    class Builder {
    public:
        Builder(Device &device) : device{device} {}
    
        Builder &addPoolSize(VkDescriptorType descriptorType, uint32_t count);
        Builder &setPoolFlags(VkDescriptorPoolCreateFlags flags);
        Builder &setMaxSets(uint32_t count);
        std::unique_ptr<DescriptorPoolManager> build() const;
    
    private:
        Device &device;
        std::vector<VkDescriptorPoolSize> poolSizes{};
        uint32_t maxSets = 1000;
        VkDescriptorPoolCreateFlags poolFlags = 0;
    };

    DescriptorPoolManager(
        Device &device,
        uint32_t maxSets,
        VkDescriptorPoolCreateFlags poolFlags,
        const std::vector<VkDescriptorPoolSize> &poolSizes);
    ~DescriptorPoolManager();
    DescriptorPoolManager(const DescriptorPoolManager &) = delete;
    DescriptorPoolManager &operator=(const DescriptorPoolManager &) = delete;
    
    bool allocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet &descriptor);
    
    void freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const;

    void resetPool();
private:
    void allocateNewPool();

    Device &device;

    uint32_t maxSets;

    VkDescriptorPoolCreateFlags poolFlags; 
    std::vector<VkDescriptorPool> descriptorPools;
    std::vector<VkDescriptorPoolSize> poolSizes;
    VkDescriptorPool freePool = VK_NULL_HANDLE;
    
    friend class Device; 
    friend class DescriptorWriter;
};

class DescriptorWriter {
 public:
  DescriptorWriter(DescriptorSetLayout &setLayout, DescriptorPoolManager &poolManager);
 
  DescriptorWriter &writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo);
  DescriptorWriter &writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo);
 
  bool build(VkDescriptorSet &set);
  void overwrite(VkDescriptorSet &set);
 
 private:
  DescriptorSetLayout &setLayout;
  DescriptorPoolManager &poolManager;
  std::vector<VkWriteDescriptorSet> writes;
};

}