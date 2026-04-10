#include "GlobalRenderSystem.hpp"

#include "../vk/Renderer.hpp"

namespace myvk {

GlobalRenderSystem::GlobalRenderSystem(Renderer& renderer) : RenderSystem(renderer)
{
    createGlobalLayouts();
    createPipelineLayout(layouts);

	PipelineConfigInfo config{};
	Pipeline::defaultPipelineConfigInfo(config);

	createPipeline(renderer.getSwapChainRenderPass(), config);
}

void GlobalRenderSystem::createGlobalLayouts() {
    uniforms.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
	for(int i = 0; i < uniforms.size(); i++) {
		uniforms[i] = std::make_unique<Buffer>(
			device, 
			sizeof(GlobalUbo),
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

}