#include "rendering/systems/WireframeRenderSystem.hpp"

#include "vk/Device.hpp"
#include "vk/Buffer.hpp"
#include "vk/Renderer.hpp"
#include "vk/RenderTarget.hpp"
#include "vk/Pipeline.hpp"
#include "vk/Swapchain.hpp"
#include "vk/Descriptors.hpp"
#include "vk/VkTexture.hpp"
#include "vk/Material.hpp"
#include "vk/Mesh.hpp"

#include "rendering/RenderState.hpp"
#include "rendering/RenderQueue.hpp"

#include "texture/Texture.hpp"


namespace myvk 
{

WireframeRenderSystem::WireframeRenderSystem(Renderer& renderer)
{
    createLayouts();

    createPipelineLayout(layouts);

	PipelineConfigInfo config{};
	Pipeline::defaultPipelineConfigInfo(config);

    config.rasterizationInfo.polygonMode = VK_POLYGON_MODE_LINE;

	createPipeline(renderer.getSwapChainRenderPass(), config);

    createDefaultMaterial();
}

WireframeRenderSystem::WireframeRenderSystem(Renderer& renderer, RenderTarget& target)
{
    createLayouts();

    createPipelineLayout(layouts);

	PipelineConfigInfo config{};
	Pipeline::defaultPipelineConfigInfo(config);

    config.rasterizationInfo.polygonMode = VK_POLYGON_MODE_LINE;

	createPipeline(target.getRenderPass(), config);
    
    createDefaultMaterial();
}

WireframeRenderSystem::~WireframeRenderSystem() = default;

}