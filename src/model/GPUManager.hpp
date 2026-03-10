#pragma once

#include "GPUMesh.hpp"
#include "GPUMaterial.hpp"
#include <memory>

namespace myvk {

class GPUManager {
private:
    Device& device;
    DescriptorPoolManager& pool;
    DescriptorSetLayout& materialLayout;
    VkPipelineLayout pipelineLayout;

    std::shared_ptr<GPUMaterial> defaultMaterial;
    void createDefaultMaterial();
public:
    GPUManager(Device& device,
               DescriptorPoolManager& pool,
               DescriptorSetLayout& layout,
               VkPipelineLayout pipelineLayout);

    std::shared_ptr<GPUMesh> createMesh(Mesh* instance, GPUMeshBufferFlags flags);
    std::shared_ptr<GPUMaterial> createMaterial(Texture2D* texture, TextureFilter filter);

    std::shared_ptr<GPUMaterial> getDefaultMaterial() {return defaultMaterial;}
};

}