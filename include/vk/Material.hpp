#pragma once

#include <memory>

struct VkCommandBuffer_T;
using VkCommandBuffer = VkCommandBuffer_T*;

struct VkDescriptorSet_T;
using VkDescriptorSet = VkDescriptorSet_T*;

struct VkPipelineLayout_T;
using VkPipelineLayout = VkPipelineLayout_T*;

namespace myvk {

class VkTexture;

class RenderSystem;
class Renderer;

class DescriptorPoolManager;
class DescriptorSetLayout;

class Material {
public:
    Material() {};

    void setRenderSystem(RenderSystem* system);

    void setAlbedo(std::unique_ptr<VkTexture> albedo);
    
    void bind(VkCommandBuffer commandBuffer) const;

    inline VkDescriptorSet& getDescriptor() { return descriptor; }
private:
    std::unique_ptr<VkTexture> albedo;

    DescriptorPoolManager* pool = nullptr;
    DescriptorSetLayout* layout = nullptr;
    VkPipelineLayout pipelineLayout = nullptr;
    VkDescriptorSet descriptor      = nullptr;
};

//struct MaterialResources : public ResourceManager<Material> {};
//using MaterialHandle = MaterialResources::ReferencedResource;

}