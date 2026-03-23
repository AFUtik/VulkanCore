#include "RenderService.hpp"
#include <iostream>
#include <stdexcept>

namespace myvk {

RenderService::RenderService(Device& device, DescriptorPoolManager& pool, DescriptorSetLayout& materialLayout) : device(device), pool(pool), materialLayout(materialLayout), container(this)
{   
    container.service = this;
    createEmptyMaterial();
}

void RenderService::createEmptyMaterial() {
    std::unique_ptr<uint8_t[]> whitePixel = std::make_unique<uint8_t[]>(4);
	whitePixel[0] = 255;
	whitePixel[1] = 255;
	whitePixel[2] = 255;
	whitePixel[3] = 255;

    Material material {std::make_shared<Texture2D>(std::move(whitePixel), 1, 1, TextureChannels::RGBA)};
    defMaterialHandle = createMaterialHandle(&material, container);
}

void RenderService::render(Handle<RenderObject> id, const glm::mat4& transform) {
    drawList.push_back( DrawCommand { transform, id.handle });
}

Handle<RenderObject> RenderService::registerRenderObject(RenderObject object, HandleContainer& container) {
    Handle<RenderObject> handle;
    handle.handle = renderables.create(object);
    container.push(handle);
    return handle;
}

void RenderService::deleteMeshDefferedDirect(Handle<DrawMesh> &handle) {
    if(!handle.valid()) {
        std::cerr << "RenderService: Failed to delete Mesh by this handle. The handle is invalid" << std::endl;
        return;
    }
    DrawMesh& mesh = meshes[handle.handle];
    if(!mesh.tag.empty()) {
        auto it = tagMeshMap.find(mesh.tag);
        tagMeshMap.erase(it);
    }

    meshes.remove(handle.handle);
    handle.destroy();
}

void RenderService::deleteMaterialDefferedDirect(Handle<DrawMaterial> &handle) {
    if(!handle.valid()) {
        std::cerr << "RenderService: Failed to delete Material by this handle. The handle is invalid" << std::endl;
        return;
    }
    else if(handle.handle == defMaterialHandle.handle) return;
    
    DrawMaterial& material = materials[handle.handle];
    if(!material.tag.empty()) {
        auto it = tagMeshMap.find(material.tag);
        tagMeshMap.erase(it);
    }

    materials.remove(handle.handle);
    handle.destroy();
}

Handle<DrawMesh> RenderService::createMeshHandle(const Mesh *mesh, HandleContainer& container, std::string tag) {
    if(!mesh) return Handle<DrawMesh>();

    DrawMesh drawMesh;

    drawMesh.gpuData = std::make_unique<GPUMesh>(device, mesh);
    drawMesh.firstVertex = 0;
    drawMesh.firstIndex  = 0;
    drawMesh.indexCount  = mesh->indices.size();
    drawMesh.vertexCount = mesh->vertices.size();

    Handle<DrawMesh> handle;
    if(!tag.empty()) {
        tagMeshMap.emplace(tag, handle);
        drawMesh.tag = tag;
    }
    container.push(handle);
    handle.handle = meshes.create(std::move(drawMesh));

    return handle;
};

Handle<DrawMaterial> RenderService::createMaterialHandle(const Material* material, HandleContainer& container, std::string tag) {
    if(material == nullptr) return defMaterialHandle; // Return default material //
    if(!material->albedo) return Handle<DrawMaterial>();

    auto texture  = std::make_shared<GPUTexture>(device, material->albedo.get(), material->albedoFilter);

    DrawMaterial drawMaterial;
    drawMaterial.gpuData  = std::make_unique<GPUMaterial>(pool, materialLayout, texture);

    Handle<DrawMaterial> handle;
    if(!tag.empty()) {
        tagMaterialMap.emplace(tag, handle);
        drawMaterial.tag = tag;
    }
    container.push(handle);
    handle.handle = materials.create(std::move(drawMaterial));
    
    return handle;
}

Handle<DrawMesh> RenderService::getMeshHandle(std::string tag) {
    auto it = tagMeshMap.find(tag);
    if(it != tagMeshMap.end()) {
        return it->second;
    }
    return Handle<DrawMesh>();
}

Handle<DrawMaterial> RenderService::getMaterialHandle(std::string tag) {
    auto it = tagMaterialMap.find(tag);
    if(it != tagMaterialMap.end()) {
        return it->second;
    }
    return Handle<DrawMaterial>();
}

void RenderService::deleteObject(Handle<RenderObject>& handle, HandleContainer& container) {
    RenderObject& object = renderables[handle.handle];
    deleteMesh(object.mesh, container);
    deleteMaterial(object.material, container);
    container.remove(UHandle::make(handle));
}

void RenderService::deleteMesh(Handle<DrawMesh>& handle, HandleContainer& container) {
    deleteMeshDefferedDirect(handle);
    container.remove(UHandle::make(handle));
}

void RenderService::deleteMaterial(Handle<DrawMaterial>& handle, HandleContainer& container) {
    deleteMaterialDefferedDirect(handle);
    container.remove(UHandle::make(handle));
}

// Handle Container //

HandleContainer::HandleContainer(RenderService* service) : service(service) {
    if(service==nullptr) {
        throw std::runtime_error("HandleContainer Cons: RenderService is nullptr.");
    }
}

void HandleContainer::remove_and_handle(UHandle handle) {
    remove(handle);
    handles.remove(handle.handle_container);
}

void HandleContainer::remove(UHandle handle) {
    if(handle.type == HandleType::Undefined) {
        std::cerr << "a Type of the Handle is Undefined." << std::endl;
        return;
    }
    if(handle.type == HandleType::Mesh_H) {
        auto typed_handle = Handle<DrawMesh> {handle.handle};
        service->deleteMeshDefferedDirect(typed_handle);
    }
    else if(handle.type == HandleType::Material_H) {
        auto typed_handle = Handle<DrawMaterial> {handle.handle};
        service->deleteMaterialDefferedDirect(typed_handle);
    }
    else if(handle.type == HandleType::RenderObject_H) {
        service->renderables.remove(handle.handle);
    }
}

HandleContainer::~HandleContainer() {
    if(service) {
        for(int i = 0; i < handles.size(); i++) {
            UHandle& u = handles[i];
            if(u.valid()) remove(handles[i]);
        }
    } else {
        throw std::runtime_error("HandleContainer Desc: RenderService is nullptr. ");
    }

    std::cout << "Successfully allocated." << std::endl;
}

};