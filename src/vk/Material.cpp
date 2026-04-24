#include "Material.hpp"

namespace myvk {

void Material::setAlbedo(std::unique_ptr<VkTexture> albedo) 
{
    VkDescriptorImageInfo imageInfo;
	imageInfo.sampler   = albedo->getSampler();
	imageInfo.imageView = albedo->getView();
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	
	DescriptorWriter(*layout, *pool)
		.writeImage(0, &imageInfo)
		.build(descriptor);
	
    this->albedo = std::move(albedo);
}

void Material::bind(VkCommandBuffer commandBuffer) const {
    vkCmdBindDescriptorSets(
        commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipelineLayout,
        1,
        1,
        &descriptor,
        0,
	    nullptr);
}

}