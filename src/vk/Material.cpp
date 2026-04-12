#include "Material.hpp"

namespace myvk {

Material::Material(DescriptorPoolManager& pool, DescriptorSetLayout& layout) : pool(pool), layout(layout)
{

}

void Material::create(Texture* albedo) 
{
    VkDescriptorImageInfo imageInfo;
	if(albedo) {
		imageInfo.sampler   = albedo->getSampler();
		imageInfo.imageView = albedo->getView();
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	}

	DescriptorWriter(layout, pool)
		.writeImage(0, &imageInfo)
		.build(descriptorSet);
	
}

void Material::bind(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, int frame) const {
    vkCmdBindDescriptorSets(
        commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipelineLayout,
        1,
        1,
        &descriptorSet.set,
        0,
	    nullptr);
}

}