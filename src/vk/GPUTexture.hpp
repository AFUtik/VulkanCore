#pragma once

#include "Device.hpp"
#include "Buffer.hpp"

#include "../model/Texture.hpp"

#include <vma/vk_mem_alloc.h>

namespace myvk {
    class GPUTexture {
    public:
        struct DeletionInfo {
            VkDevice device;
            VkSampler sampler;
            VkImageView view;
            VkImage image;

            VmaAllocator allocator;
            VmaAllocation allocation;
        };

        GPUTexture(Device& device, Texture2D* texture, TextureFilter filter = TextureFilter::Linear);
        ~GPUTexture();

        GPUTexture(const GPUTexture&) = delete;
		GPUTexture& operator=(const GPUTexture&) = delete;

        VkSampler getSampler() {return sampler;}
        VkImageView getView() {return view;}
    private:
        GPUTexture::DeletionInfo getDeletionInfo();

        void imageMemBarrier(VkCommandBuffer CmdBuf, VkImageLayout OldLayout, VkImageLayout NewLayout, int layerCount);
        
        void transitionImageLayout(VkImageLayout OldLayout, VkImageLayout NewLayout, int LayerCount);

		void updateGPUTextureImage(int layerCount, const void* pPixels);

		void createImageView(VkImageAspectFlags AspectFlags);

		void createGPUTextureSampler(VkFilter MinFilter, VkFilter MaxFilter, VkSamplerAddressMode AddressMode);

		void createImage();

        void createGPUTextureFromData(const void* pPixels);

		void createGPUTexture(Texture2D* texture);

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