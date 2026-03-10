#pragma once

#include "model/Model.hpp"
#include "model/GPUMesh.hpp"
#include "model/GPUMaterial.hpp"

#include "Descriptors.hpp"

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

	std::unique_ptr<GPUMesh> original;
};

struct RenderObject {
    Handle<DrawMesh> mesh;
    Handle<GPUMaterial> material;
    glm::mat4 transform;
};

class RenderScene {
private:
    Device& device;
    DescriptorPoolManager& pool;
    DescriptorSetLayout& materialLayout;
    VkPipelineLayout pipelineLayout;
    
    std::vector<RenderObject> renderables;
    std::vector<DrawMesh> meshes;
    std::vector<std::unique_ptr<GPUMaterial>> materials;

    std::vector<Handle<RenderObject>> dirtyObjects;

	std::unordered_map<Mesh*, Handle<DrawMesh>> meshConvert;
    //std::unordered_map<vkutil::Material*, Handle<GPUMaterial>> materialConvert;
public:
    RenderScene(Device& device, DescriptorPoolManager& pool, DescriptorPoolManager& materialLayout, VkPipelineLayout pipelineLayout);

    Handle<RenderObject> registerObject(MeshObject* meshObject);
    void deleteObject(Handle<RenderObject> objectId);
    
    void updateTransform(Handle<RenderObject> objectId, const glm::mat4& localToWorld);

	Handle<DrawMesh> getMeshHandle(Mesh* m);
    //Handle<GPUMaterial> getMaterialHandle(vkutil::Material* m);
};

}
