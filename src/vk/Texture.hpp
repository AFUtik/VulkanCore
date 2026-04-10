#pragma once

#include "Device.hpp"
#include "Buffer.hpp"

#include "../model/Texture.hpp"

#include <vma/vk_mem_alloc.h>

namespace myvk {
    class Texture {
    public:
        Texture(Device& device, Texture2D* texture, TextureFilter filter = TextureFilter::Linear);
        ~Texture();

        Texture(const Texture&) = delete;
		Texture& operator=(const Texture&) = delete;

        VkSampler getSampler() {return sampler;}
        VkImageView getView() {return view;}
    private:
        void imageMemBarrier(VkCommandBuffer CmdBuf, VkImageLayout OldLayout, VkImageLayout NewLayout, int layerCount);
        
        void transitionImageLayout(VkImageLayout OldLayout, VkImageLayout NewLayout, int LayerCount);

		void updateTextureImage(int layerCount, const void* pPixels);

		void createImageView(VkImageAspectFlags AspectFlags);

		void createTextureSampler(VkFilter MinFilter, VkFilter MaxFilter, VkSamplerAddressMode AddressMode);

		void createImage();

        void createTextureFromData(const void* pPixels);

		void createTexture(Texture2D* texture);

        bool isCubemap = false;
        int imageWidth, imageHeight, imageChannels;

        // cpu texture info //
        TextureFilter filter;
        TextureChannels channels;

        VkFormat format;
        VkImage image;
        VmaAllocation vmaAllocation;
        VkImageView view;
        VkSampler sampler;
        VkImageLayout imageLayout;
        Device& device;

        friend class Device;
    };
}