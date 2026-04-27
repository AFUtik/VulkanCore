#include "vk/VkTexture.hpp"
#include "vk/Device.hpp"
#include "vk/Buffer.hpp"

#include <stdexcept>

namespace myvk {

VkTexture::VkTexture(
	const uint8_t* pixels, 
    uint32_t width, uint32_t height,
    uint32_t channels, 
    TextureFilter filter) : device(Device::instance()), imageWidth(width), imageHeight(height), channels(channels)
{
	createTexture(pixels, channels, filter);
};

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

VkTexture::~VkTexture() {
	device.free<VkTexture>(this);
}

void VkTexture::createTexture(const uint8_t* pixels, uint32_t channels, TextureFilter filter) {
	if(channels == 4) 
	{
		format = VK_FORMAT_R8G8B8A8_SRGB;
	} 
	else if(channels == 3) 
	{
		format = VK_FORMAT_R8G8B8_SRGB;
	}
	else if (channels == 2) 
	{
		format = VK_FORMAT_R8G8_UNORM;
	}
	else if(channels == 1) 
	{
		format = VK_FORMAT_R8_UNORM;
	}
	
	// Step #1: create the image object and populate it with pixels
	createImage();

	int LayerCount = isCubemap ? 6 : 1;
	updateTextureImage(LayerCount, pixels);

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
	createTextureSampler(sampler, MinFilter, MaxFilter, AddressMode);
}

void VkTexture::createImage()
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


void VkTexture::updateTextureImage(int layerCount, const void* pPixels)
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

	device.transitionImageLayout(image, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, layerCount);

	device.copyBufferToImage(stagingBuffer.getBuffer(), image, imageWidth, imageHeight, layerCount);
	
	device.transitionImageLayout(image, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, layerCount);
}

void VkTexture::createTextureSampler(VkSampler& sampler, VkFilter MinFilter, VkFilter MaxFilter, VkSamplerAddressMode AddressMode)
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

void VkTexture::createImageView(VkImageAspectFlags AspectFlags) 
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
	if(channels == 4 || channels == 3) {
		viewInfo.components = {
			.r = VK_COMPONENT_SWIZZLE_IDENTITY,
			.g = VK_COMPONENT_SWIZZLE_IDENTITY,
			.b = VK_COMPONENT_SWIZZLE_IDENTITY,
			.a = VK_COMPONENT_SWIZZLE_IDENTITY
		};
	}
	else if(channels == 1) {
		viewInfo.components = {
			.r = VK_COMPONENT_SWIZZLE_ONE,
			.g = VK_COMPONENT_SWIZZLE_ONE,
			.b = VK_COMPONENT_SWIZZLE_ONE,
			.a = VK_COMPONENT_SWIZZLE_R
		};
	}
	else if(channels == 2) {
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