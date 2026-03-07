#pragma once

#include "Camera.hpp"
#include "Descriptors.hpp"
#include "RenderSystem.hpp"

#include "model/GPUMaterial.hpp"
#include "gui/GUIRendering.hpp"
#include "Fonts.hpp"

namespace myvk {

class UIRenderSystem : public RenderSystem {
private:
    std::unique_ptr<GUIRender> guiRender;
    std::unique_ptr<GPUMaterial> emptyMaterial; // 1x1 white texture

    DescriptorPoolManager* descriptorPool;
    std::unique_ptr<DescriptorSetLayout> uiSetLayout;
	std::unique_ptr<DescriptorSetLayout> uiMaterialSetLayout;
	std::vector<DescriptorSetData> uiDescriptorSets;
	std::vector<VkDescriptorSetLayout> uiLayouts;
	std::vector<std::unique_ptr<Buffer>> uiUniform;

    FontHandler fontHandler;
	std::unique_ptr<Font> curFont = nullptr;
	std::unique_ptr<Text> curText = nullptr;

    void createUILayouts(Camera* camera);
    void createEmptyMaterial();
    void createUIFont();
public:
    UIRenderSystem(
        Device &device, 
        VkRenderPass renderPass, 
        DescriptorPoolManager* descriptorPool,
        GUIContext* guiContext, Camera* camera);

    void render(FrameInfo& frameInfo) override;
};

}