#pragma once

#include "Swapchain.hpp"
#include "Descriptors.hpp"
#include "Material.hpp"

#include <string>
#include <vector>
#include <memory>

namespace myvk {
    struct FrameInfo;
    struct TextureView;

    struct RenderTarget {
        RenderTarget(SwapChain* swapchain, VkExtent2D extent);
        ~RenderTarget();

        VkFramebuffer getFrameBuffer(int index) { return framebuffers[index]; }
        VkRenderPass getRenderPass() { return renderPass; }
        VkImageView getImageView(int index) { return imageViews[index]; }
        size_t imageCount() { return images.size(); }
        VkFormat getImageFormat() { return imageFormat; }
        VkExtent2D getExtent() { return extentTarget; }
        uint32_t width() { return extentTarget.width; }
        uint32_t height() { return extentTarget.height; }

        //void blitImage(SwapChain* swapchain);
        void beginRenderPass(FrameInfo& frame);
        void endRenderPass(FrameInfo& frame);

        Material* getFramebufferTexture(FrameInfo& frame);
        void createFramebufferTexture(DescriptorPoolManager* desc_pool, DescriptorSetLayout* desc_layout, VkPipelineLayout pipelineLayout);
    private:
        void createImages();
        void createDepthResources();
        void createRenderPass();
        void createFramebuffers();

        VkFormat findDepthFormat();
    
        Device& device = Device::instance();
        
        VkRenderPass renderPass;
        VkExtent2D extentTarget;

        VkFormat imageFormat;
        VkFormat depthFormat;
        
        std::vector<VkImage>       depthImages;
        std::vector<VmaAllocation> depthImageAllocs;
        std::vector<VkImageView>   depthImageViews;

        std::vector<VkImage>       images;
        std::vector<VmaAllocation> imageAllocs;
        std::vector<VkImageView>   imageViews;

        std::vector<std::unique_ptr<Material>> screenTextures;
        std::vector<VkSampler> screenSamplers;

        std::vector<VkFramebuffer> framebuffers;
    };
}