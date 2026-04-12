#pragma once

#include "Descriptors.hpp"
#include "Device.hpp"

#include "Texture.hpp"

#include <memory>

namespace myvk {

class Material {
public:
    Material(DescriptorPoolManager& pool, DescriptorSetLayout& layout);

    void create(Texture* albedo);
    
    void bind(VkCommandBuffer commandBuffer, VkPipelineLayout layout, int frame) const;

    DescriptorSetData& getDescriptorSetData() {return descriptorSet;}
private:
    DescriptorPoolManager& pool;
    DescriptorSetLayout& layout;
    DescriptorSetData descriptorSet;
};

}