
#pragma once

#include "GUIContext.hpp"

#include "../model/GPUMesh.hpp"
#include "../model/Model.hpp"

#include <memory>

namespace myvk {

struct GUIContentModel {
    uint32_t id = 0;
    uint32_t version = 0;
    std::shared_ptr<Model> model;
};

struct GUIWindowRender {
    GUIWindow* window;
    uint32_t version = 0;
    bool visible = true;
    int zOrder = 0;
    
    std::vector<GUIContentModel> models;

    void buildMeshes(Device& device);
};

struct GUIRender {
    std::vector<std::unique_ptr<GUIWindowRender>> windowToRender;
    GUIContext* context;
    
    GUIRender(GUIContext* guiContext);

    void fetchContext(Device& device);
};

} 