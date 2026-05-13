#pragma once

#include "vulkan/vulkan_core.h"
#include <vulkan/vulkan.h>

struct VmaAllocation_T;
using VmaAllocation = VmaAllocation_T*;

namespace myvk {
    class Device;

    enum TextureFilter
    { 
        Linear,
        Nearest,
        LinearMipmap,
        NearestMipmap
    };

    class VkTexture {
    public:
        VkTexture(
            const uint8_t* pixels, 
            uint32_t width, uint32_t height,
            uint32_t channels, 
            TextureFilter filter = TextureFilter::Linear);

        ~VkTexture();

        VkTexture(const VkTexture&) = delete;
        VkTexture& operator=(const VkTexture&) = delete;

        VkSampler   getSampler() {return sampler;}
        VkImageView getView() {return view;}
    private:
        void createTextureSampler(
            VkSampler& sampler, 
            VkFilter MinFilter, 
            VkFilter MaxFilter, 
            VkSamplerAddressMode AddressMode);

		void updateTextureImage(int layerCount, const void* pPixels);

		void createImageView(VkImageAspectFlags AspectFlags);

		void createImage();
        
		void createTexture(const uint8_t* pixels, uint32_t channels, TextureFilter filter);

        bool isCubemap = false;

        VkImage image = VK_NULL_HANDLE;
        VmaAllocation vmaAllocation  = VK_NULL_HANDLE;
        VkImageView view  = VK_NULL_HANDLE;
        VkSampler sampler  = VK_NULL_HANDLE;
        VkImageLayout imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        uint32_t imageWidth, imageHeight, channels;
        VkFormat format;
        
        Device& device;
        friend class Device;
    };

}