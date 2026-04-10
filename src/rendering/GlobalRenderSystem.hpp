#pragma once

#include "RenderSystem.hpp"

class Camera;
class Renderer;

namespace myvk {

class GlobalRenderSystem : public RenderSystem {
private:
    void createGlobalLayouts();
public:
    GlobalRenderSystem(Renderer& renderer);
};

}