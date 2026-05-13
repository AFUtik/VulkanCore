#pragma once 

#include "vk/Mesh.hpp"

namespace myvk {
    class Material;
}

struct RenderBatch
{ 
    myvk::Mesh* mesh              = nullptr;
    myvk::Material* material      = nullptr;
    myvk::InstanceData* instances = nullptr;
    uint32_t instanceCount = 0;
};

struct RenderQueue
{ 
    std::vector<RenderBatch> batchQueue;
};