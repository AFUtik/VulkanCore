#pragma once 

#include "vk/Mesh.hpp"
#include "rendering/BaseMesh.hpp"

namespace myvk {
    class Material;
}

struct RenderBatch
{ 
    myvk::Mesh* mesh              = nullptr;
    myvk::Material* material      = nullptr;

    const InstanceData* instances = nullptr;
    uint32_t instanceCount = 0;
};

struct RenderQueue
{ 
    std::vector<RenderBatch> batchQueue;
};