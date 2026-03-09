#include "GUIRendering.hpp"
#include "../model/Mesh.hpp"
#include "../model/GPUMesh.hpp"

namespace myvk {

void GUIWindowRender::buildMesh() {
    std::shared_ptr<Model> model = std::make_shared<Model>();
    MeshTools::Quad quad;

    // Window mesh //
    quad.r = window->windowColor.r; 
    quad.g = window->windowColor.g; 
    quad.b = window->windowColor.b; 
    quad.a = window->windowColor.a;
    quad.x2 = (float)window->window.width;
    quad.y2 = (float)window->window.height;

    MeshTools::createQuad(*model->meshInstance.get(), quad);
    models.push_back(model);

    // Footer model //
    std::shared_ptr<Model> footerModel = std::make_shared<Model>();
    quad = MeshTools::Quad();

    quad.r = window->footerColor.r; 
    quad.g = window->footerColor.g; 
    quad.b = window->footerColor.b; 
    quad.a = window->footerColor.a;
    quad.y1 = (float)(window->window.height - window->footer.height);
    quad.x2 = (float)window->window.width;
    quad.y2 = (float)window->window.height;
    MeshTools::createQuad(*footerModel->meshInstance.get(), quad);

    footerModel->transform.translate({0.0, 0.0, 1.0});
    models.push_back(footerModel);
}

void GUIWindowRender::fetchWindow() {
    for(auto& model : models) {
        model->transform.setPosition({window->pos.x, window->pos.y, 0.0});
        //cmodel.model->transform.setZ((float)window->zOrder);
    }
}

GUIRenderer::GUIRenderer(GUIContext* context) : context(context) {

}

void GUIRenderer::buildDrawList() {
    for(auto& window : windowToRender) for(auto model : window->models) renderSystem->addToDrawList(model.get());
}

void GUIRenderer::fetchContext() {
    for(std::shared_ptr<GUIWindow>& window : context->windows()) {
        auto windowRender = std::make_unique<GUIWindowRender>();
        windowRender->window = window.get();
        windowRender->version = window->version;
        windowRender->buildMesh();

        windowToRender.push_back(std::move(windowRender));
    }
}

}
