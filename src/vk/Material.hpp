#pragma once

#include "Descriptors.hpp"
#include "VkTexture.hpp"

#include "management/ResourceManager.hpp"

#include <memory>

namespace myvk {

class RenderSystem;
class Renderer;

class Material {
public:
    Material() {};

    void setRenderSystem(RenderSystem* system);

    void setAlbedo(std::unique_ptr<VkTexture> albedo);
    
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