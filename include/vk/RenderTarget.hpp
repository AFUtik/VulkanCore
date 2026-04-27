#pragma once

#include <vulkan/vulkan.h>

#include <vector>
#include <memory>

struct VmaAllocation_T;
using VmaAllocation = VmaAllocation_T*;

namespace myvk {
    class Device;

    struct FrameInfo;

    class SwapChain;
    class RenderSystem; 
    class Material;

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
        void createFramebufferTexture(RenderSystem* system);
    private:
        void createImages();
        void createDepthResources();
        void createRenderPass();
        void createFramebuffers();

        VkFormat findDepthFormat();
    
        Device& device;
        
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