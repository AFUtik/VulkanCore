#pragma once

#include "management/ResourceManager.hpp"

#include "../vk/Mesh.hpp"
#include "../vk/Material.hpp"

#include "../model/Mesh.hpp"
#include "../model/Texture.hpp"

struct RenderMeshes : public ResourceManager<myvk::Mesh> 
{
    void create(const Mesh& mesh);
};

struct RenderMaterials : public ResourceManager<myvk::Material> 
{
    void create(const Texture2D& texture);
}; 

using RenderMesh = Resource<myvk::Mesh>;
using RenderMaterial = Resource<myvk::Material>;
using InstanceData = myvk::InstanceData;

struct MeshInstance {
    RenderMesh mesh;

    MeshInstance(RenderMesh mesh, InstanceData data = {});
    ~MeshInstance();

    void update(const InstanceData& data);
private:
    uint32_t index;
};

struct RenderBatch {
    RenderMesh mesh;
    RenderMaterial material;
};

struct RenderObject {
    MeshInstance mesh;
    RenderMaterial material;
    AABB renderBounds;
};

struct RenderService {
    void render(const RenderBatch& batch);
    void render(const RenderObject& object);
};