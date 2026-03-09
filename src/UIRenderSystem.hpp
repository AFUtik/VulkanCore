#pragma once

#include "Camera.hpp"
#include "Descriptors.hpp"
#include "FrameInfo.hpp"
#include "RenderSystem.hpp"

#include "model/GPUMaterial.hpp"
#include "gui/GUIRendering.hpp"
#include "Fonts.hpp"

namespace myvk {

class UIRenderSystem : public RenderSystem {
private:
    void createUILayouts();
public:
    UIRenderSystem(Device &device, VkRenderPass renderPass, DescriptorPoolManager* descriptorPool, FrameInfo& frame);
};

}