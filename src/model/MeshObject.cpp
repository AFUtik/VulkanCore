#include "MeshObject.hpp"

MeshObject::~MeshObject() {
    //if(renderScene) renderService->deleteObjectDeffered(Handle<RenderObject>(renderId));
}

void MeshObject::setMesh(std::shared_ptr<Mesh> mesh) {
    //this->mesh = mesh;
    //if(Handle<RenderObject>::valid(renderId)) {
    //    renderService->updateMeshData(this, Handle<RenderObject>(renderId));
    //}
}

void MeshObject::setMaterial(std::shared_ptr<Material> material) {
    this->material = material;
    // renderScene->updateMeshData(this, Handle<RenderObject>(renderId));
}

void MeshObject::updateMesh() {
    //renderService->updateMeshData(this, Handle<RenderObject>(renderId));
}

void MeshObject::updateMaterial() {
    // renderScene->updateMeshData(this, Handle<RenderObject>(renderId));
}
