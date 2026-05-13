#include "rendering/systems/BaseRenderSystem.hpp"

#include "vk/Device.hpp"
#include "vk/Buffer.hpp"
#include "vk/RenderSystem.hpp"
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

BaseRenderSystem::BaseRenderSystem(Renderer& renderer, PipelineConfigInfo& config) : RenderSystem(renderer) 
{
    createLayouts();

    createPipelineLayout(layouts);

	createPipeline(renderer.getSwapChainRenderPass(), config);

    createDefaultMaterial();
}

BaseRenderSystem::BaseRenderSystem(Renderer& renderer, RenderTarget& target, PipelineConfigInfo& config) : RenderSystem(renderer)
{
    createLayouts();

    createPipelineLayout(layouts);

	createPipeline(target.getRenderPass(), config);
    
    createDefaultMaterial();
}

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
	const size_t maxInstances = 100000;

	globalUniforms.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
	for(int i = 0; i < globalUniforms.size(); i++) {
		globalUniforms[i] = std::make_unique<Buffer>(
			device, 
			sizeof(GlobalUniformBuffer),
			1,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, 
			VMA_MEMORY_USAGE_CPU_TO_GPU);
		globalUniforms[i]->map();
	}

	stagingInstanceSsbo.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
	for (int i = 0; i < stagingInstanceSsbo.size(); i++) {
		stagingInstanceSsbo[i] = std::make_unique<Buffer>(
			device,
			sizeof(myvk::InstanceData),
			maxInstances,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			VMA_MEMORY_USAGE_CPU_TO_GPU
		);
		stagingInstanceSsbo[i]->map();
	}

	setLayout = DescriptorSetLayout::Builder(device)
		.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
		.addBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
		.build();
	layouts.push_back(setLayout->getDescriptorSetLayout());

	materialSetLayout = DescriptorSetLayout::Builder(device)
		.addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		.build();
	layouts.push_back(materialSetLayout->getDescriptorSetLayout());

	descriptorSets.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
	for(int i = 0; i < descriptorSets.size(); i++) {
		auto bufferInfo = globalUniforms[i]->descriptorInfo();
		auto instanceSsboInfo = stagingInstanceSsbo[i]->descriptorInfo();
		DescriptorWriter(*setLayout, *descriptorPool)
			.writeBuffer(0, &bufferInfo)
			.writeBuffer(1, &instanceSsboInfo)
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

void BaseRenderSystem::render(RenderState& state, RenderBatch& batch) 
{
    auto& frame = state.frame;

	stagingInstanceSsbo[frame.frameIndex]->writeToBuffer(batch.instances, sizeof(myvk::InstanceData) * batch.instanceCount);
	globalUniforms[frame.frameIndex]->writeToBuffer(&state.projview);

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
	
	batch.material->bind(frame.commandBuffer);
	batch.mesh->draw(frame.commandBuffer, batch.instanceCount);
}

}