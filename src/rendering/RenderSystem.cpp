#include "RenderSystem.hpp"

#include "../vk/Renderer.hpp"
#include "../vk/FrameInfo.hpp"
#include "../vk/Pipeline.hpp"
#include "../vk/Texture.hpp"
#include "../model/Mesh.hpp"
#include "vulkan/vulkan_core.h"

#include <memory>
#include <stdexcept>

#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>
#include <iostream>

namespace myvk {

RenderSystem::RenderSystem(Renderer& renderer) : renderer(renderer), descriptorPool(renderer.getDescriptorPool())
{
	createPipelineLayout(layouts);

	PipelineConfigInfo config{};
	Pipeline::defaultPipelineConfigInfo(config);

	createPipeline(renderer.getSwapChainRenderPass(), config);
}

RenderSystem::~RenderSystem() {
	vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
}

/*
void RenderSystem::createEmptyMaterial() {
	std::unique_ptr<uint8_t[]> whitePixel = std::make_unique<uint8_t[]>(4);
	whitePixel[0] = 255;
	whitePixel[1] = 255;
	whitePixel[2] = 255;
	whitePixel[3] = 255;

	std::shared_ptr<Texture2D> texture     = std::make_shared<Texture2D>(std::move(whitePixel), 1, 1, TextureChannels::RGBA);
	std::shared_ptr<GPUTexture> gpuTexture = std::make_shared<GPUTexture>(device, texture.get());
	emptyMaterial = std::make_unique<GPUMaterial>(*descriptorPool, *materialSetLayout, gpuTexture);
}
*/

void RenderSystem::createPipelineLayout(const std::vector<VkDescriptorSetLayout>& layouts_) {
	VkPushConstantRange pushConstantRange{};
	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(PushConstantData);

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(layouts_.size());
	pipelineLayoutInfo.pSetLayouts = layouts_.data();
	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
	if (vkCreatePipelineLayout(device.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
		VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}
}

void RenderSystem::createPipeline(VkRenderPass renderPass ,PipelineConfigInfo& pipelineConfig) {
	assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

	pipelineConfig.renderPass     = renderPass;
	pipelineConfig.pipelineLayout = pipelineLayout;
	pipeline = std::make_unique<Pipeline>(
		device,
		absolutePath + "resources/shaders/shader.vert.spv",
		absolutePath + "resources/shaders/shader.frag.spv",
		pipelineConfig);
	
	PipelineConfigInfo newInfo = pipelineConfig;

	newInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_LINE;
	newInfo.inputAssemblyInfo.topology    = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
	pipelineLineMode = std::make_unique<Pipeline>(
		device,
		absolutePath + "resources/shaders/shader.vert.spv",
		absolutePath + "resources/shaders/shader.frag.spv",
		newInfo);
}

void RenderSystem::setProjview(const glm::mat4& projview) {
	const FrameInfo& frame = renderer.frameInfo();

	uniforms[frame.frameIndex]->writeToBuffer(&projview);
	uniforms[frame.frameIndex]->flush();
}

void RenderSystem::render(Mesh* mesh, Material* mat, const glm::mat4& model) {
	const FrameInfo& frame = renderer.frameInfo();
	pipeline->bind(frame.commandBuffer);

	vkCmdBindDescriptorSets(
		frame.commandBuffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		pipelineLayout,
		0,
		1,
		&descriptorSets[frame.frameIndex].set,
		0, nullptr
	);
	
	mat->bind (frame.commandBuffer, pipelineLayout, frame.frameIndex);
	mesh->draw(frame.commandBuffer);
}

}