#pragma once

#include "RenderSystem.hpp"

class Camera;

namespace myvk {
    class RenderTarget;
    class Renderer;

    class GlobalRenderSystem : public RenderSystem {
    private:
        void createGlobalLayouts();
    public:
        GlobalRenderSystem(Renderer& renderer, VkRenderPass renderPass);
    };
}