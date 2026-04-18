#pragma once

#include "Device.hpp"
#include "Buffer.hpp"

#include "management/ResourceManager.hpp"

#include "../model/Texture.hpp"

namespace myvk {
    class Texture {
    public:
        Texture() {};
        ~Texture();

        Texture(Texture&& other) noexcept;
        Texture& operator=(Texture&& other) noexcept;

        VkSampler   getSampler() {return sampler;}
        VkImageView getView() {return view;}
        
        static void imageMemBarrier(
            VkImage image,
            VkFormat format,
            VkCommandBuffer CmdBuf,
            VkImageLayout OldLayout, 
            VkImageLayout NewLayout, 
            int layerCount);
            
        static void transitionImageLayout(
            Device& device,
            VkImage image,
            VkFormat format,
            VkImageLayout OldLayout, 
            VkImageLayout NewLayout, 
            int LayerCount);
        
        static void createTextureSampler(
            Device& device, 
            VkSampler& sampler, 
            VkFilter MinFilter, 
            VkFilter MaxFilter, 
            VkSamplerAddressMode AddressMode);

        void create(Texture2D* texture, TextureFilter filter = TextureFilter::Linear);
    private:
		void updateTextureImage(int layerCount, const void* pPixels);

		void createImageView(VkImageAspectFlags AspectFlags);

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

        Device& device = Device::instance();

        friend class Device;
    };

}