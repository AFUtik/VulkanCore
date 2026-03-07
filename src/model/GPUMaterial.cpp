#include "GPUMaterial.hpp"

namespace myvk {

GPUMaterial::GPUMaterial(DescriptorPoolManager& pool,
                         DescriptorSetLayout& layout,
                         std::shared_ptr<GPUTexture> albedo) : pool(pool), layout(layout), albedo(albedo)
{
   create(albedo);
}

void GPUMaterial::create(std::shared_ptr<GPUTexture> albedo) 
{
    VkDescriptorImageInfo imageInfo;
	if(albedo) {
		imageInfo.sampler   = albedo->getSampler();
		imageInfo.imageView = albedo->getView();
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	}

    descriptorSets.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
	for(int i = 0; i < descriptorSets.size(); i++) {
		DescriptorWriter(layout, pool)
			.writeImage(0, &imageInfo)
			.build(descriptorSets[i]);
	}
}

void GPUMaterial::bind(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, int frame) const {
    vkCmdBindDescriptorSets(
        commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipelineLayout,
        1,
        1,
        &descriptorSets[frame].set,
        0,
	    nullptr);
}

}