#pragma once

#include "RenderSystem.hpp"

namespace myvk {

class GlobalRenderSystem : public RenderSystem {
private:
    GlobalUbo ubo{};

    void createGlobalLayouts();
public:
    GlobalRenderSystem(Device &device, VkRenderPass renderPass, DescriptorPoolManager* descriptorPool, FrameInfo& frame);

    void renderGlobal();
};

}