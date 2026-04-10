#include "Texture.hpp"

#include <stdexcept>

namespace myvk {

int GetBytesPerTexFormat(VkFormat Format)
{
	switch (Format)
	{
	case VK_FORMAT_R8_SINT:
	case VK_FORMAT_R8_UNORM:
		return 1;
	case VK_FORMAT_R16_SFLOAT:
		return 2;
	case VK_FORMAT_R16G16_SFLOAT:
	case VK_FORMAT_R16G16_SNORM:
	case VK_FORMAT_B8G8R8A8_UNORM:
	case VK_FORMAT_R8G8B8A8_UNORM:
	case VK_FORMAT_R8G8B8A8_SNORM:	
	case VK_FORMAT_R8G8B8A8_SRGB:
		return 4;
	case VK_FORMAT_R16G16B16A16_SFLOAT:
		return 4 * sizeof(uint16_t);
	case VK_FORMAT_R32G32B32_SFLOAT:
		return 3 * sizeof(float);
	case VK_FORMAT_R8G8B8_SRGB:
		return 3;
	case VK_FORMAT_R32G32B32A32_SFLOAT:
		return 4 * sizeof(float);
	default:
		printf("Unknown format %d\n", Format);
		exit(1);
	}

	return 0;
}

bool HasStencilComponent(VkFormat Format)
{
	return ((Format == VK_FORMAT_D32_SFLOAT_S8_UINT) || 
		    (Format == VK_FORMAT_D24_UNORM_S8_UINT));
}


Texture::Texture(Device& device, Texture2D* texture, TextureFilter filter) : 
	device(device),
	imageWidth(texture->width),
	imageHeight(texture->height),
	imageChannels(texture->channels),
	filter(filter),
	channels((TextureChannels)texture->channels)
{
    createTexture(texture);
}

Texture::~Texture() {
	device.allocate<Texture>(this);
}

// Copied from the "3D Graphics Rendering Cookbook"
void Texture::imageMemBarrier(VkCommandBuffer CmdBuf, VkImageLayout OldLayout, VkImageLayout NewLayout, int layerCount)
{
	VkImageMemoryBarrier Barrier = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.pNext = NULL,
		.srcAccessMask = 0,
		.dstAccessMask = 0,
		.oldLayout = OldLayout,
		.newLayout = NewLayout,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = image,
		.subresourceRange = VkImageSubresourceRange {
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = (uint32_t)layerCount
		}
	};

	VkPipelineStageFlags sourceStage = VK_PIPELINE_STAGE_NONE;
	VkPipelineStageFlags destinationStage = VK_PIPELINE_STAGE_NONE;

	if (NewLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL ||
		(format == VK_FORMAT_D16_UNORM) ||
		(format == VK_FORMAT_X8_D24_UNORM_PACK32) ||
		(format == VK_FORMAT_D32_SFLOAT) ||
		(format ==  VK_FORMAT_S8_UINT) ||
		(format == VK_FORMAT_D16_UNORM_S8_UINT) ||
		(format == VK_FORMAT_D24_UNORM_S8_UINT))
	{
		Barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

		if (HasStencilComponent(format)) {
			Barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
	}
	else {
		Barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}

	if (OldLayout == VK_IMAGE_LAYOUT_UNDEFINED && NewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		Barrier.srcAccessMask = 0;
		Barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (OldLayout == VK_IMAGE_LAYOUT_UNDEFINED && NewLayout == VK_IMAGE_LAYOUT_GENERAL) {
		Barrier.srcAccessMask = 0;
		Barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}

	if (OldLayout == VK_IMAGE_LAYOUT_UNDEFINED && 
		NewLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		Barrier.srcAccessMask = 0;
		Barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	} /* Convert back from read-only to updateable */
	else if (OldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && NewLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		Barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
		Barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	} /* Convert from updateable Texture to shader read-only */
	else if (OldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && 
		     NewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		Barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		Barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	} /* Convert depth Texture from undefined state to depth-stencil buffer */
	else if (OldLayout == VK_IMAGE_LAYOUT_UNDEFINED && NewLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		Barrier.srcAccessMask = 0;
		Barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	} /* Wait for render pass to complete */
	else if (OldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && NewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		Barrier.srcAccessMask = 0; // VK_ACCESS_SHADER_READ_BIT;
		Barrier.dstAccessMask = 0;
		/*
				sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		///		destinationStage = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
				destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		*/
		sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	} /* Convert back from read-only to color attachment */
	else if (OldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && NewLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
		Barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
		Barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	} /* Convert from updateable Texture to shader read-only */
	else if (OldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && NewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		Barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		Barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	} /* Convert back from read-only to depth attachment */
	else if (OldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && NewLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		Barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
		Barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		destinationStage = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	} /* Convert from updateable depth Texture to shader read-only */
	else if (OldLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL && NewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		Barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		Barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (OldLayout == VK_IMAGE_LAYOUT_UNDEFINED && NewLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
		Barrier.srcAccessMask = 0;
		Barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	} 
	else if (OldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && NewLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) {
		Barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		Barrier.dstAccessMask = 0;

		sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		destinationStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	}
	else {
		printf("Unknown Barrier case\n");
		exit(1);
	}

	vkCmdPipelineBarrier(CmdBuf, sourceStage, destinationStage, 
		                 0, 0, NULL, 0, NULL, 1, &Barrier);
}

void Texture::transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout, int layerCount)
{
    VkCommandBuffer m_copyCmdBuf = device.beginSingleTimeCommands();

	imageMemBarrier(m_copyCmdBuf, oldLayout, newLayout, layerCount);

	device.endSingleTimeCommands(m_copyCmdBuf);
}

void Texture::createTexture(Texture2D* texture) {
	if(channels == TextureChannels::RGBA) 
	{
		format = VK_FORMAT_R8G8B8A8_SRGB;
	} 
	else if(channels == TextureChannels::RGB) 
	{
		format = VK_FORMAT_R8G8B8_SRGB;
	}
	else if(channels == TextureChannels::Grayscale) 
	{
		format = VK_FORMAT_R8_UNORM;
	}
	else if (channels == TextureChannels::GrayscaleAlpha) 
	{
		format = VK_FORMAT_R8G8_UNORM;
	}
	createTextureFromData(texture->raw());
}

void Texture::createTextureFromData(const void* pPixels)
{
	// Step #1: create the image object and populate it with pixels
	createImage();

	int LayerCount = isCubemap ? 6 : 1;
	updateTextureImage(LayerCount, pPixels);

	// Step #2: create the image view
	VkImageAspectFlags AspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
	createImageView(AspectFlags);

	VkFilter MinFilter;
	VkFilter MaxFilter;
	if(filter == TextureFilter::Linear) {
		MinFilter = VK_FILTER_LINEAR;
		MaxFilter = VK_FILTER_LINEAR;
	} else if(filter == TextureFilter::Nearest) {
		MinFilter = VK_FILTER_NEAREST;
		MaxFilter = VK_FILTER_NEAREST;
	}
	
	VkSamplerAddressMode AddressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT;

	// Step #3: create the Texture sampler
	createTextureSampler(MinFilter, MaxFilter, AddressMode);

	printf("Texture from data created\n");
}

void Texture::createImage()
{
	/*VkImageFormatProperties imageFormatProperties;
	vkGetPhysicalDeviceImageFormatProperties(m_physDevices.Selected().m_physDevice,
		TexFormat,
		VK_IMAGE_TYPE_2D,
		VK_IMAGE_TILING_OPTIMAL,
		UsageFlags,
		VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT,
		&imageFormatProperties);*/

	VkImageCreateInfo imageInfo = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.pNext = NULL,
		.flags = isCubemap ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : (VkImageCreateFlags)0,
		.imageType = VK_IMAGE_TYPE_2D,
		.format = format,
		.extent = VkExtent3D {.width = static_cast<uint32_t>(imageWidth), .height = static_cast<uint32_t>(imageHeight), .depth = 1 },
		.mipLevels = 1,
		.arrayLayers = isCubemap ? 6u : 1u,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.tiling = VK_IMAGE_TILING_OPTIMAL,
		.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = NULL,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
	};
    device.createImageWithInfo(imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image, vmaAllocation);
}


void Texture::updateTextureImage(int layerCount, const void* pPixels)
{
	int BytesPerPixel = GetBytesPerTexFormat(format);

	VkDeviceSize layerSize = imageWidth * imageHeight * BytesPerPixel;	 
	VkDeviceSize imageSize = layerCount * layerSize;

	Buffer stagingBuffer(
		device,
		imageSize,
		1,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		VMA_MEMORY_USAGE_CPU_ONLY
	);
	stagingBuffer.map();
	stagingBuffer.writeToBuffer(pPixels, imageSize);
	stagingBuffer.unmap();

	transitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, layerCount);

	device.copyBufferToImage(stagingBuffer.getBuffer(), image, imageWidth, imageHeight, layerCount);
	
	transitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, layerCount);
}

void Texture::createTextureSampler(VkFilter MinFilter, VkFilter MaxFilter, VkSamplerAddressMode AddressMode)
{
	VkSamplerCreateInfo SamplerInfo = {
		.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.magFilter = MinFilter,
		.minFilter = MaxFilter,
		.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
		.addressModeU = AddressMode,
		.addressModeV = AddressMode,
		.addressModeW = AddressMode,
		.mipLodBias = 0.0f,
		.anisotropyEnable = VK_FALSE,
		.maxAnisotropy = 1,
		.compareEnable = VK_FALSE,
		.compareOp = VK_COMPARE_OP_ALWAYS,
		.minLod = 0.0f,
		.maxLod = 0.0f,
		.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
		.unnormalizedCoordinates = VK_FALSE
	};
    if(vkCreateSampler(device.device(), &SamplerInfo, VK_NULL_HANDLE, &sampler) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create image sampler");
    } 
}

void Texture::createImageView(VkImageAspectFlags AspectFlags) 
{
	VkImageViewCreateInfo viewInfo =
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.image = image,
		.viewType = isCubemap ? VK_IMAGE_VIEW_TYPE_CUBE : VK_IMAGE_VIEW_TYPE_2D,
		.format = format,
		.subresourceRange = {
			.aspectMask = AspectFlags,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = isCubemap ? 6u : 1u
		}
	};

	// Managing image view channels //
	if(channels == TextureChannels::RGBA || channels == TextureChannels::RGB) {
		viewInfo.components = {
			.r = VK_COMPONENT_SWIZZLE_IDENTITY,
			.g = VK_COMPONENT_SWIZZLE_IDENTITY,
			.b = VK_COMPONENT_SWIZZLE_IDENTITY,
			.a = VK_COMPONENT_SWIZZLE_IDENTITY
		};
	}
	else if(channels == TextureChannels::Grayscale) {
		viewInfo.components = {
			.r = VK_COMPONENT_SWIZZLE_ONE,
			.g = VK_COMPONENT_SWIZZLE_ONE,
			.b = VK_COMPONENT_SWIZZLE_ONE,
			.a = VK_COMPONENT_SWIZZLE_R
		};
	}
	else if(channels == TextureChannels::GrayscaleAlpha) {
		viewInfo.components = {
			.r = VK_COMPONENT_SWIZZLE_R,
			.g = VK_COMPONENT_SWIZZLE_R,
			.b = VK_COMPONENT_SWIZZLE_R,
			.a = VK_COMPONENT_SWIZZLE_G
		};
	}

    if(vkCreateImageView(device.device(), &viewInfo, NULL, &view) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create image view");
    }
}

}