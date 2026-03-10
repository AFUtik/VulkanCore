#pragma once

#include "model/Model.hpp"
#include "model/GPUMesh.hpp"
#include "model/GPUMaterial.hpp"

#include "Descriptors.hpp"
#include "FreeList.hpp"

#include <memory>

namespace myvk {

template <typename T>
struct Handle {
    uint32_t handle;
};

struct DrawMesh {
	uint32_t firstVertex;
	uint32_t firstIndex;
	uint32_t indexCount;
	uint32_t vertexCount;
	bool isMerged;

	std::unique_ptr<GPUMesh> gpuData;
    Mesh* original;
    uint32_t refCount = 0;
};

struct DrawMaterial {
    std::unique_ptr<GPUMaterial> gpuData;
    Material* original;
    uint32_t refCount = 0;
};

struct RenderObject {
    MeshObject* meshObject;
    Handle<DrawMesh> mesh;
    Handle<DrawMaterial> material;
    glm::mat4 transform;
};

class RenderScene {
private:
    Device& device;
    DescriptorPoolManager& pool;
    DescriptorSetLayout& materialLayout;
    VkPipelineLayout pipelineLayout;
    
    FreeList<RenderObject> renderables;
    FreeList<DrawMesh> meshes;
    FreeList<DrawMaterial> materials;

    std::vector<Handle<RenderObject>> dirtyObjects;

	std::unordered_map<Mesh*, Handle<DrawMesh>> meshConvert;
    std::unordered_map<Material*, Handle<DrawMaterial>> materialConvert;
    
    Handle<DrawMesh> getMeshHandle(Mesh* m);
    Handle<DrawMaterial> getMaterialHandle(Material* m);

    void deleteMeshDeffered(Handle<DrawMesh> &handle);
    void deleteMaterialDeffered(Handle<DrawMaterial> &handle);
public:
    RenderScene(Device& device, DescriptorPoolManager& pool, DescriptorPoolManager& materialLayout, VkPipelineLayout pipelineLayout);

    Handle<RenderObject> registerObject(MeshObject* meshObject);
    void updateObject(Handle<RenderObject> objectId);

    void deleteObjectDeffered(Handle<RenderObject> objectId);
    
    void updateTransform(Handle<RenderObject> objectId, const glm::mat4& localToWorld);

    inline RenderObject& getRenderObject(Handle<RenderObject> handle) {return renderables[handle.handle];}
};

}
