#include "gfx/vk/RenderSystem.hpp"
#include "gfx/vk/Device.hpp"
#include "gfx/vk/Renderer.hpp"
#include "gfx/vk/Pipeline.hpp"

#include <memory>
#include <stdexcept>

#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>

namespace vk {

RenderSystem::RenderSystem(Renderer& renderer) : device(Device::instance()), descriptorPool(renderer.getDescriptorPool()) {}

RenderSystem::~RenderSystem() {
	vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
}

void RenderSystem::createPipelineLayout(const std::vector<VkDescriptorSetLayout>& layouts_) {
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(layouts_.size());
	pipelineLayoutInfo.pSetLayouts = layouts_.data();
	if (vkCreatePipelineLayout(device.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
		VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}
}

void RenderSystem::addPipeline(VkRenderPass renderPass, PipelineConfigInfo& pipelineConfig, uint32_t& pipelineId) {
	assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

	pipelineConfig.renderPass     = renderPass;
	pipelineConfig.pipelineLayout = pipelineLayout;
	pipelines.push_back(std::make_unique<Pipeline>(pipelineConfig));
	pipelineId = static_cast<uint32_t>(pipelines.size()-1);
}

}