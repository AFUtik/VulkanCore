
#include "GUIRendering.hpp"
#include "../model/Mesh.hpp"
#include "../model/GPUMesh.hpp"

namespace myvk {

void GUIWindowRender::buildMeshes(Device& device) {
    MeshInstance mesh;

    mesh.vertices.push_back({float(window->posX + window->windowWidth), float(window->posY + window->windowHeight), 0.0f, 0, 0, 1, 1, 1, 1});
    mesh.vertices.push_back({float(window->posX + window->windowWidth), float(window->posY),     0.0f, 0, 0, 1, 1, 1, 1});
    mesh.vertices.push_back({float(window->posX), float(window->posY),     0.0f, 0, 0, 1, 1, 1, 1});
    mesh.vertices.push_back({float(window->posX), float(window->posY + window->windowHeight), 0.0f, 0, 0, 1, 1, 1, 1});

    mesh.indices.push_back(0);
    mesh.indices.push_back(1);
    mesh.indices.push_back(2);
    mesh.indices.push_back(2);
    mesh.indices.push_back(3);
    mesh.indices.push_back(0);

    std::shared_ptr<Model> model = std::make_shared<Model>();
    model->mesh = std::make_shared<GPUMesh>(device, mesh);  
    
    models.emplace_back(0, 0, model);
}

GUIRender::GUIRender(GUIContext* context) : context(context) {

}

void GUIRender::fetchContext(Device& device) {
    for(std::shared_ptr<GUIWindow>& window : context->windows()) {
        auto windowRender = std::make_unique<GUIWindowRender>();
        windowRender->window = window.get();
        windowRender->version = window->version;
        windowRender->buildMeshes(device);
        windowToRender.push_back(std::move(windowRender));
    }
}

}
