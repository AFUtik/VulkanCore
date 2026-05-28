#include "gfx/vk/Material.hpp"
#include "gfx/vk/Texture.hpp"
#include "gfx/vk/Descriptors.hpp"
#include "gfx/vk/RenderSystem.hpp"

namespace vk {

void Material::setAlbedo(std::unique_ptr<Texture> albedo) 
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

void Material::setRenderSystem(RenderSystem* system) 
{
    pool = system->getDescriptorPool();
    layout = system->getMaterialSetLayout();
    pipelineLayout = system->getPipelineLayout();
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