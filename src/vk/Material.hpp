#pragma once

#include "Descriptors.hpp"
#include "VkTexture.hpp"

#include "management/ResourceManager.hpp"

#include <memory>

namespace myvk {

class Material {
public:
    Material() {};

    void setAlbedo(std::unique_ptr<VkTexture> albedo);
    
    inline void setDescriptorPool(DescriptorPoolManager* manager) {pool = manager;  }
    inline void setDescriptorLayout(DescriptorSetLayout* layout)  {this->layout = layout; }
    inline void setPipelineLayout(VkPipelineLayout layout) {pipelineLayout = layout;}
    
    void bind(VkCommandBuffer commandBuffer) const;

    VkDescriptorSet& getDescriptor() {return descriptor;}
private:
    std::unique_ptr<VkTexture> albedo;

    DescriptorPoolManager* pool = nullptr;
    DescriptorSetLayout* layout = nullptr;
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    VkDescriptorSet descriptor      = VK_NULL_HANDLE;
};

struct MaterialResources : public ResourceManager<Material> {};
using MaterialHandle = MaterialResources::ReferencedResource;

}