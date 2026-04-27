#pragma once 

#include "vk/Mesh.hpp"

namespace myvk {
    class Material;
}

struct RenderBatch
{ 
    myvk::Mesh* mesh;
    myvk::Material* material;
    //std::vector<myvk::InstanceData>& instances;
};

struct RenderQueue
{ 
    std::vector<RenderBatch> batchQueue;
};