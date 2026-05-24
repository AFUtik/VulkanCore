#pragma once

#include "BaseMesh.hpp"

namespace vk 
{
    struct Material;
};

namespace gfx
{
    struct IMesh;
}

struct RenderBatch
{ 
    vk::Mesh* mesh              = nullptr;
    vk::Material* material      = nullptr;

    const InstanceData* instances = nullptr;
    uint32_t instanceCount = 0;
};

struct RenderQueue
{ 
    std::vector<RenderBatch> batchQueue;
};