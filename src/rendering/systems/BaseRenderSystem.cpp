#include "BaseRenderSystem.hpp"

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

#include "../RenderState.hpp"

#include "texture/Texture.hpp"

namespace myvk 
{

BaseRenderSystem::BaseRenderSystem(Renderer& renderer) : RenderSystem(renderer) 
{
    createLayouts();

    createPipelineLayout(layouts);

	PipelineConfigInfo config{};
	Pipeline::defaultPipelineConfigInfo(config);

	createPipeline(renderer.getSwapChainRenderPass(), config);

    createDefaultMaterial();
}

BaseRenderSystem::BaseRenderSystem(Renderer& renderer, RenderTarget& target) : RenderSystem(renderer)
{
    createLayouts();

    createPipelineLayout(layouts);

	PipelineConfigInfo config{};
	Pipeline::defaultPipelineConfigInfo(config);

	createPipeline(target.getRenderPass(), config);
    
    createDefaultMaterial();
}

BaseRenderSystem::~BaseRenderSystem() = default;

void BaseRenderSystem::createLayouts() 
{
	uniforms.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
	for(int i = 0; i < uniforms.size(); i++) {
		uniforms[i] = std::make_unique<Buffer>(
			device, 
			sizeof(Ubo),
			1,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, 
			VMA_MEMORY_USAGE_CPU_TO_GPU);
		uniforms[i]->map();
	}

	setLayout = DescriptorSetLayout::Builder(device)
		.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
		.build();
	layouts.push_back(setLayout->getDescriptorSetLayout());

	materialSetLayout = DescriptorSetLayout::Builder(device)
		.addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		.build();
	layouts.push_back(materialSetLayout->getDescriptorSetLayout());

	descriptorSets.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
	for(int i = 0; i < descriptorSets.size(); i++) {
		auto bufferInfo = uniforms[i]->descriptorInfo();
		DescriptorWriter(*setLayout, *descriptorPool)
			.writeBuffer(0, &bufferInfo)
			.build(descriptorSets[i]);
	}
}

void BaseRenderSystem::createDefaultMaterial() 
{
	defaultMaterial = std::make_unique<Material>();

	std::unique_ptr<uint8_t[]> whitePixel = std::make_unique<uint8_t[]>(4);
	whitePixel[0] = 255;
	whitePixel[1] = 255;
	whitePixel[2] = 255;
	whitePixel[3] = 255;

    Texture defaultTex(std::move(whitePixel), 1, 1, TextureChannels::RGBA);
    defaultMaterial->setRenderSystem(this);
    defaultMaterial->setAlbedo(
		std::make_unique<myvk::VkTexture>(
			defaultTex.raw(), defaultTex.width, defaultTex.height, defaultTex.height, TextureFilter::Nearest
		)
	);
}

Material* BaseRenderSystem::getDefaultMaterial()
{ 
	return defaultMaterial.get();
}

void BaseRenderSystem::render(RenderState& state, Mesh* mesh, Material* mat) {
    auto& frame = state.frame;
	uniforms[frame.frameIndex]->writeToBuffer(&state.projview);
	pipeline->bind(frame.commandBuffer);

	vkCmdBindDescriptorSets(
		frame.commandBuffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		pipelineLayout,
		0,
		1,
		&descriptorSets[frame.frameIndex],
		0, nullptr
	);
	
	mat->bind(frame.commandBuffer);
	mesh->draw(frame.commandBuffer);
}

}