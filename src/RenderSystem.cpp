#include "RenderSystem.hpp"
#include "Pipeline.hpp"

#include <functional>
#include <stdexcept>
#include <array>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

using namespace myvk;

RenderSystem::RenderSystem(Device &device, VkRenderPass renderPass, std::vector<VkDescriptorSetLayout> layouts) : device(device) {
	createPipelineLayout(layouts);

	PipelineConfigInfo config{};
	Pipeline::defaultPipelineConfigInfo(config);

	createPipeline(renderPass, config);
}

RenderSystem::~RenderSystem() {
	vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
}

void RenderSystem::addModel(std::shared_ptr<Model> model) {
	models.push_back(model);
}

void RenderSystem::createPipelineLayout(const std::vector<VkDescriptorSetLayout>& layouts) {
	VkPushConstantRange pushConstantRange{};
	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(PushConstantData);

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(layouts.size());
	pipelineLayoutInfo.pSetLayouts = layouts.data();
	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
	if (vkCreatePipelineLayout(device.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
		VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}
}

void RenderSystem::createPipeline(VkRenderPass renderPass ,PipelineConfigInfo& pipelineConfig) {
	assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

	pipelineConfig.renderPass = renderPass;
	pipelineConfig.pipelineLayout = pipelineLayout;
	pipeline = std::make_unique<Pipeline>(
		device,
		absolutePath + "resources/shaders/shader.vert.spv",
		absolutePath + "resources/shaders/shader.frag.spv",
		pipelineConfig);
}

void RenderSystem::render(FrameInfo& frame) {
	pipeline->bind(frame.commandBuffer);
	vkCmdBindDescriptorSets(
		frame.commandBuffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		pipelineLayout,
		0,
		1,
		&frame.globalDescriptorSet,
		0, nullptr
	);
	for(auto& model : models) {
		model->material->bind(frame.commandBuffer, pipelineLayout, frame.frameIndex);

		vkCmdPushConstants(
			frame.commandBuffer,
			pipelineLayout,
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
			0,
			sizeof(PushConstantData),
			&model->transform.model()
		);
		model->mesh->bind(frame.commandBuffer);
		model->mesh->draw(frame.commandBuffer);
	}
}