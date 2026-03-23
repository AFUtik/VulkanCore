#pragma once

#include "../model/MeshObject.hpp"
#include "../vk/GPUTexture.hpp"
#include "../vk/GPUMesh.hpp"
#include "../vk/GPUMaterial.hpp"
#include "../vk/Descriptors.hpp"

#include "../FreeList.hpp"

#include <limits>
#include <memory>
#include <deque>
#include <stdexcept>
#include <vector>
#include <unordered_map>
#include <type_traits>

enum HandleType {
    Mesh_H,
    Material_H, 
    RenderObject_H,
    Undefined
};

template <typename T>
struct Handle {
    static constexpr uint32_t INVALID_HANDLE   = std::numeric_limits<uint32_t>().max();
    static constexpr uint32_t DESTROYED_HANDLE = INVALID_HANDLE-1;

    uint32_t handle = INVALID_HANDLE; // DEFAULT INVALID HANDLE //
    uint32_t handle_container = INVALID_HANDLE;

    inline bool valid() {return handle < DESTROYED_HANDLE;}
    static inline bool valid(uint32_t value) {return value < DESTROYED_HANDLE;}
    
    inline void destroy() {handle = DESTROYED_HANDLE;}
};

struct DrawMesh {
	uint32_t firstVertex;
	uint32_t firstIndex;
	uint32_t indexCount;
	uint32_t vertexCount;
	bool isMerged;

	std::unique_ptr<myvk::GPUMesh> gpuData;
    std::string tag;
};

struct DrawMaterial {
    std::unique_ptr<myvk::GPUMaterial> gpuData;
    std::string tag;
};

struct RenderObject {
    Handle<DrawMesh> mesh;
    Handle<DrawMaterial> material;
    std::string tag;
};

// Universal Handle with no template FOR handle container //
struct UHandle : public Handle<void> 
{
    HandleType type = HandleType::Undefined;

    template<typename T>
    static UHandle make(Handle<T> handle) {
        UHandle u;
        u.handle = handle.handle;

        if constexpr (std::is_same_v<T, RenderObject>) {
            u.type = HandleType::RenderObject_H;
        }
        else if constexpr (std::is_same_v<T, DrawMesh>) {
            u.type = HandleType::Mesh_H;
        }
        else if constexpr (std::is_same_v<T, DrawMaterial>) {
            u.type = HandleType::Material_H;
        }
        return u;
    }
};

struct DrawCommand {
    glm::mat4 transform;
    uint32_t  object_id;
};

class HandleContainer {
    RenderService* service;
    FreeList<UHandle> handles;
    
    void remove_and_handle(UHandle handle);
public:
    HandleContainer(RenderService* service);
    ~HandleContainer();

    template<typename T>
    void push(Handle<T>& handle) 
    {
        UHandle u = UHandle::make(handle);
        handle.handle_container = (uint32_t)handles.create(u);
    }

    void remove(UHandle handle);

    friend class RenderService;
};

class RenderSystem;

class RenderService {
private:
    myvk::Device& device;
    myvk::DescriptorPoolManager& pool;
    myvk::DescriptorSetLayout& materialLayout;
    
    FreeList<RenderObject> renderables;
    FreeList<DrawMesh> meshes;
    FreeList<DrawMaterial> materials;

    std::unordered_map<std::string, Handle<RenderObject>> tagObject;
    std::unordered_map<std::string, Handle<DrawMesh>> tagMeshMap;
    std::unordered_map<std::string, Handle<DrawMaterial>> tagMaterialMap;

    std::vector<DrawCommand> drawList;

    // Creates material where albedo is 1x1 white texture, sets by default if there's no material in meshObject //
    void createEmptyMaterial(); 
    
    void deleteMeshDefferedDirect(Handle<DrawMesh> &handle);
    void deleteMaterialDefferedDirect(Handle<DrawMaterial> &handle);

    HandleContainer container; 
    Handle<DrawMaterial> defMaterialHandle;

    friend class HandleContainer;
    friend class myvk::RenderSystem;
public:
    RenderService(myvk::Device& device, myvk::DescriptorPoolManager& pool, myvk::DescriptorSetLayout& materialLayout);

    void render(Handle<RenderObject> id, const glm::mat4& transform);

    Handle<RenderObject> registerRenderObject(
        RenderObject object, 
        HandleContainer& container);
    
    Handle<DrawMesh> createMeshHandle(
        const Mesh* mesh, 
        HandleContainer& container,
        std::string tag = "");

    Handle<DrawMaterial> createMaterialHandle(
        const Material* material, 
        HandleContainer& container,
        std::string tag = "");

    Handle<DrawMesh> getMeshHandle(std::string tag = "");
    Handle<DrawMaterial> getMaterialHandle(std::string tag = "");
    
    void deleteObject(Handle<RenderObject>& handle, HandleContainer& container);
    void deleteMaterial(Handle<DrawMaterial>& handle, HandleContainer& container);
    void deleteMesh(Handle<DrawMesh>& handle, HandleContainer& container);
};
