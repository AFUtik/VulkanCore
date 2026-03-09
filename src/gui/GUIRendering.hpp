
#pragma once

#include "GUIContext.hpp"
#include "../RenderSystem.hpp"

#include <memory>

namespace myvk {

class GUIWindowRender {
public:
    uint32_t version = 0;
    
    std::vector<std::shared_ptr<Model>> models;

    inline bool isVisible() {return window->visible;};
    void fetchWindow();
private:
    void buildMesh();
    GUIWindow* window;

    friend class GUIRenderer;
};

class GUIRenderer : public ObjectRenderer {
public:
    std::vector<std::unique_ptr<GUIWindowRender>> windowToRender;
    GUIContext* context;
    
    GUIRenderer(GUIContext* guiContext);

    void fetchContext();
    void buildDrawList() override;
};

} 