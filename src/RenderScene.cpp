#include "RenderScene.hpp"

namespace myvk {

Handle<RenderObject> RenderScene::registerObject(MeshObject* meshObject) {
    RenderObject render;
    render.mesh = getMeshHandle(meshObject->mesh.get());
    render.material = getMaterialHandle(meshObject->material.get());
    render.transform = meshObject->transform.model();

    Handle<RenderObject> handle;
    handle.handle = renderables.create(render);

    return handle;
}

void RenderScene::updateObject(Handle<RenderObject> objectId) {
    RenderObject& renderObject = renderables[objectId.handle];
    
}

void RenderScene::deleteObjectDeffered(Handle<RenderObject> handle) {
    RenderObject& renderObject = renderables[handle.handle];

    deleteMeshDeffered(renderObject.mesh);
    deleteMaterialDeffered(renderObject.material);

    renderables.remove(handle.handle);
}

void RenderScene::deleteMeshDeffered(Handle<DrawMesh> &handle) {
    if(!handle.handle) return;

    DrawMesh& drawMesh = meshes[handle.handle];
    drawMesh.refCount--;

    if(drawMesh.refCount == 0) {
        auto it = meshConvert.find(drawMesh.original);
        if(it != meshConvert.end()) meshConvert.erase(it);

        meshes.remove(handle.handle);
        handle.handle = 0;
    }
}

void RenderScene::deleteMaterialDeffered(Handle<DrawMaterial> &handle) {
    if(!handle.handle) return;

    DrawMaterial& drawMaterial = materials[handle.handle];
    drawMaterial.refCount--;

    if(drawMaterial.refCount == 0) {
        auto it = materialConvert.find(drawMaterial.original);
        if(it != materialConvert.end()) materialConvert.erase(it);

        materials.remove(handle.handle);
        handle.handle = 0;
    }
}

Handle<DrawMesh> RenderScene::getMeshHandle(Mesh* mesh) {
    auto it = meshConvert.find(mesh);
    if(it != meshConvert.end()) {
        DrawMesh& drawMesh = meshes[it->second.handle];
        drawMesh.refCount++;
        
        return it->second;
    }
    else {
        if(!mesh) return Handle<DrawMesh>(0);

        DrawMesh drawMesh;
        drawMesh.original = mesh;
        drawMesh.gpuData = std::make_unique<GPUMesh>(device, *mesh);
        drawMesh.firstVertex = 0;
        drawMesh.firstIndex  = 0;
        drawMesh.indexCount  = mesh->indices.size();
        drawMesh.vertexCount = mesh->vertices.size();

        Handle<DrawMesh> handle;
        handle.handle = meshes.create(std::move(drawMesh));

        return handle;
    }
};

Handle<DrawMaterial> RenderScene::getMaterialHandle(Material* material) {
    auto it = materialConvert.find(material);
    if(it != materialConvert.end()) {
        return it->second;
    }
    else {
        if(!material->albedo) return Handle<DrawMaterial>(0);

        auto texture  = std::make_shared<GPUTexture>(device, material->albedo.get());

        DrawMaterial drawMaterial;
        drawMaterial.original = material;
        drawMaterial.gpuData = std::make_unique<GPUMaterial>(pool, materialLayout, texture);
        drawMaterial.refCount++;

        Handle<DrawMaterial> handle;
        handle.handle = materials.create(std::move(drawMaterial));

        return handle;
    }
}

};