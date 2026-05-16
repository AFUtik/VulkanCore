#include "vk/RenderSystem.hpp"
#include "vk/Device.hpp"
#include "vk/Renderer.hpp"
#include "vk/Pipeline.hpp"

#include <memory>
#include <stdexcept>

#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>

namespace myvk {

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

void RenderSystem::createPipeline(VkRenderPass renderPass, PipelineConfigInfo& pipelineConfig) {
	assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

	pipelineConfig.renderPass     = renderPass;
	pipelineConfig.pipelineLayout = pipelineLayout;
	pipeline = std::make_unique<Pipeline>(
		"/home/afutik/cplusplus/VulkanCore/resources/shaders/shader.vert.spv",
		"/home/afutik/cplusplus/VulkanCore/resources/shaders/shader.frag.spv",
		pipelineConfig);
}

}