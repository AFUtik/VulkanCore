#include "Renderer.hpp"
#include <memory>

Renderer::Renderer()
    : vkRenderer(), 
      vkRenderSystem(vkRenderer, vkRenderer.getSwapChainRenderPass())
{
    createDefaultMaterial();
}

void Renderer::createDefaultMaterial() 
{
    std::unique_ptr<uint8_t[]> whitePixel = std::make_unique<uint8_t[]>(4);
	whitePixel[0] = 255;
	whitePixel[1] = 255;
	whitePixel[2] = 255;
	whitePixel[3] = 255;

    Texture defaultTex(std::move(whitePixel), 1, 1, TextureChannels::RGBA);

    auto vkDefaultTex = std::make_unique<myvk::VkTexture>();
	vkDefaultTex->create(&defaultTex);
    
    vkDefaultMat.setDescriptorPool(vkRenderer.getDescriptorPool());
	vkDefaultMat.setDescriptorLayout(vkRenderSystem.getMaterialSetLayout());
	vkDefaultMat.setPipelineLayout(vkRenderSystem.getPipelineLayout());
    vkDefaultMat.setAlbedo(std::move(vkDefaultTex));
}

myvk::MaterialHandle Renderer::materialInstance() 
{
    myvk::MaterialHandle handle = myvk::MaterialResources::instance().create<myvk::MaterialHandle>(myvk::Material());
    myvk::Material& mat = handle.get();

    mat.setDescriptorPool(vkRenderer.getDescriptorPool());
	mat.setDescriptorLayout(vkRenderSystem.getMaterialSetLayout());
	mat.setPipelineLayout(vkRenderSystem.getPipelineLayout());

    return handle;
}