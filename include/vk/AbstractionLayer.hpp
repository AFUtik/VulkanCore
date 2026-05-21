#pragma once

/*

#include <cassert>
#include <vector>

#include "collections/Freelist.hpp"
#include "texture/Texture.hpp"
#include "Handle.hpp"

#include "vk/VkTexture.hpp"
#include "vk/Mesh.hpp"
#include "vk/Material.hpp"
#include "vk/Pipeline.hpp"
#include "vk/RenderSystem.hpp"
#include "vk/Renderer.hpp"

struct MeshDescriptor
{
    std::uint8_t vertexStride;
    std::uint8_t indexStride;
    std::uint8_t instanceStride;
};

struct MeshData
{
    const void* vertices  = nullptr;
    const void* indices   = nullptr;
    const void* instances = nullptr;

    std::uint32_t verticesCount  = 0;
    std::uint32_t indicesCount   = 0;
    std::uint32_t instancesCount = 0;
};

struct VkResourceManager {

    template<typename T, typename... Args>
    StaticHandle<myvk::RenderSystem> CreateRenderSystem(Args&&... args)
    {
        static_assert(std::derived_from<T, myvk::RenderSystem>);

        auto ptr = std::make_unique<T>(
            renderer,
            std::forward<Args>(args)...
        );

        auto idx = rsystems.size();
        rsystems.push_back(std::move(ptr));

        return StaticHandle<myvk::RenderSystem>(idx);
    }

    Handle<myvk::Mesh> CreateMesh(const MeshData& data, StaticHandle<myvk::RenderSystem>) 
    {
        auto mesh = myvk::Mesh{};
        if(data.verticesCount) mesh.createVertexBuffer(
            data.vertices, 
            data.verticesCount);

        if(data.indicesCount) mesh.createIndexBuffer(
            data.indices, 
            data.indicesCount);

        if(data.instancesCount) mesh.createInstanceBuffer(
            data.instances, 
            data.instancesCount);

        auto* block      = new ResourceBlock{};
        block->refCount  = 1;
        block->weakCount = 1;
        block->index     = meshPool_.allocate(std::move(mesh));
        block->alive     = true;
        block->destroy   = [](VkResourceManager* mgr, ResourceBlock* b) {
            b->alive = false;
            mgr->meshPool_.free(b->index);
        };

        Handle<myvk::Mesh> h;
        h.block_   = block;
        h.manager_ = this;
        return h;
    }

    void UpdateMesh(Handle<myvk::Mesh>& handle, const MeshData& data)
    {
        myvk::Mesh* mesh = handle.Get();
        if(data.verticesCount) mesh->updateVertexBuffer(
            data.vertices, 
            data.verticesCount);

        if(data.indicesCount) mesh->updateIndexBuffer(
            data.indices, 
            data.indicesCount);

        if(data.instancesCount) mesh->updateInstanceBuffer(
            data.instances, 
            data.instancesCount);
    }

    Handle<myvk::VkTexture> CreateTexture(Texture& texture, myvk::TextureFilter filter)
    {
        myvk::VkTexture vktex(
            texture.raw(), 
            texture.width, 
            texture.height, 
            texture.channels, 
            filter);

        auto* block      = new ResourceBlock{};
        block->refCount  = 1;
        block->weakCount = 1;
        block->index     = texturePool_.allocate(std::move(vktex));
        block->alive     = true;
        block->destroy   = [](VkResourceManager* mgr, ResourceBlock* b) {
            b->alive = false;
            mgr->meshPool_.free(b->index);
        };

        Handle<myvk::VkTexture> h;
        h.block_   = block;
        h.manager_ = this;
        return h;
    }

    void* GetRaw(ResourceBlock* block) {
        if (!block || !block->alive) return nullptr;
        return meshPool_[block->index];
    }
private:
    myvk::Renderer renderer;

    FreeList<myvk::Mesh>             meshPool_;
    FreeList<myvk::VkTexture>        texturePool_;
    FreeList<myvk::Material>         materialPool_;
    FreeList<myvk::MaterialInstance> materialInstancePool_;

    FreeList<myvk::Pipeline,     64> pipelinePool_; 
    std::vector<std::unique_ptr<myvk::RenderSystem>> rsystems;
};

template<typename T>
T* Handle<T>::Get() const {
    assert(IsValid() && "Dereferencing invalid handle");
    return static_cast<T*>(manager_->GetRaw(block_));
}

extern VkResourceManager vulkanRM;

*/