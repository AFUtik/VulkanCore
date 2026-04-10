#pragma once

#include "Descriptors.hpp"
#include "Device.hpp"

#include "Texture.hpp"

#include <memory>

namespace myvk {

class Material {
public:
    Material(DescriptorPoolManager& pool,
             DescriptorSetLayout& layout,
             std::shared_ptr<Texture> albedo);

    void create(std::shared_ptr<Texture> albedo);
                
    void bind(VkCommandBuffer commandBuffer, VkPipelineLayout layout, int frame) const;
private:
    DescriptorPoolManager& pool;
    DescriptorSetLayout& layout;
    
    std::vector<DescriptorSetData> descriptorSets;

    std::shared_ptr<Texture> albedo;
};

}