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
#include "vk/Shader.hpp"

#include "rendering/RenderState.hpp"
#include "rendering/RenderQueue.hpp"
#include "rendering/BaseMesh.hpp"

#include "texture/Texture.hpp"

namespace myvk 
{

BaseRenderSystem::BaseRenderSystem(Renderer& renderer) : RenderSystem(renderer)
{
    createLayouts();
    createPipelineLayout(layouts);
    
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
			sizeof(InstanceData),
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

void BaseRenderSystem::flushRenderQueue(RenderState& state) 
{
	auto& frame = state.frame;
	for(const auto& batch : renderQueue.batchQueue)
	{
		const size_t instancesSizeBytes = sizeof(InstanceData) * batch.instanceCount;

		stagingInstanceSsbo[frame.frameIndex]->writeToBuffer(batch.instances, instancesSizeBytes, instnaceOffsetBytes);
		globalUniforms[frame.frameIndex]->writeToBuffer(&state.projview);

		pipelines[state.pipeline]->bind(frame.commandBuffer);

		vkCmdBindDescriptorSets(
			frame.commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipelineLayout,
			0,
			1,
			&descriptorSets[frame.frameIndex],
			0, 
			nullptr
		);
		
		batch.material->bind(frame.commandBuffer);
		batch.mesh->draw(frame.commandBuffer, batch.instanceCount, instanceOffset);

		instanceOffset+=batch.instanceCount;
		instnaceOffsetBytes+=instancesSizeBytes;
	}
	renderQueue.batchQueue.clear();
}

std::vector<VkVertexInputBindingDescription> BaseRenderSystem::getBindingDescriptions() {
	std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);

	bindingDescriptions[0].binding = 0;
	bindingDescriptions[0].stride = sizeof(Vertex);
	bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return bindingDescriptions;
}

std::vector<VkVertexInputAttributeDescription> BaseRenderSystem::getAttributeDescriptions() {
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions(3);

	size_t location = 0;
	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = location;
	attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[0].offset = 0;
	location++;

	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = location;
	attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[1].offset = offsetof(Vertex, u);
	location++;

	attributeDescriptions[2].binding = 0;
	attributeDescriptions[2].location = location;
	attributeDescriptions[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	attributeDescriptions[2].offset = offsetof(Vertex, r);
	location++;

	return attributeDescriptions;
}

}