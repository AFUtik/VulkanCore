#pragma once

#include "RenderSystem.hpp"

class Camera;

namespace myvk {

class GlobalRenderSystem : public RenderSystem {
private:
    void createGlobalLayouts();
public:
    GlobalRenderSystem(Device &device, VkRenderPass renderPass, DescriptorPoolManager* descriptorPool, FrameInfo& frame);
};

}