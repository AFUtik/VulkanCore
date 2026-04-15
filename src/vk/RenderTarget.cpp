#include "RenderTarget.hpp"
#include "FrameInfo.hpp"
#include "Texture.hpp"

#include <array>
#include <memory>

namespace myvk {
    RenderTarget::RenderTarget(VkFormat imageFormat, VkExtent2D extent, Color clearColor) : extentTarget(extent), clearColor(clearColor) {
        this->imageFormat = imageFormat;
        
        createImages();
        createRenderPass();
        createDepthResources();
        createFramebuffers();
    }

    RenderTarget::~RenderTarget() {
        vkDeviceWaitIdle(device.device()); 

        for (int i = 0; i < images.size(); i++) {
            vkDestroyImageView(device.device(), imageViews[i], nullptr);
            vmaDestroyImage(device.allocator(), images[i], imageAllocs[i]);
        }

        for (int i = 0; i < depthImages.size(); i++) {
            vkDestroyImageView(device.device(), depthImageViews[i], nullptr);
            vmaDestroyImage(device.allocator(), depthImages[i], depthImageAllocs[i]);
        }

        for (auto framebuffer : framebuffers) {
            vkDestroyFramebuffer(device.device(), framebuffer, nullptr);
        }

        for(int i = 0; i < images.size(); i++) {
            if(screenTextures[i]) {
                vkDestroySampler(device.device(), screenSamplers[i], nullptr);
            }
        }

        vkDestroyRenderPass(device.device(), renderPass, nullptr);
    }

    void RenderTarget::createFramebufferTexture(DescriptorPoolManager* desc_pool, DescriptorSetLayout* desc_layout) {
        screenTextures.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
        screenSamplers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

        for(int i = 0; i < images.size(); i++) {
            screenTextures[i] = std::make_unique<Material>(*desc_pool, *desc_layout);
            Texture::createTextureSampler(
                device, 
                screenSamplers[i],
                VK_FILTER_NEAREST,
                VK_FILTER_NEAREST,
                VK_SAMPLER_ADDRESS_MODE_REPEAT);
            
            VkDescriptorImageInfo imageInfo;
            imageInfo.sampler   = screenSamplers[i];
            imageInfo.imageView = imageViews[i];
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            
            DescriptorWriter(*desc_layout, *desc_pool)
                .writeImage(0, &imageInfo)
                .build(screenTextures[i]->getDescriptorSetData());
        }
    }

    Material* RenderTarget::getFramebufferTexture(FrameInfo& frame) {
        return screenTextures[frame.frameIndex].get();
    }

    void RenderTarget::createImages() {
        images.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
        imageViews.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
        imageAllocs.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

        for (size_t i = 0; i < images.size(); i++)
        {
            VkImageCreateInfo imageInfo{};
            imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageInfo.imageType = VK_IMAGE_TYPE_2D;
            imageInfo.extent.width = extentTarget.width;
            imageInfo.extent.height = extentTarget.height;
            imageInfo.extent.depth = 1;
            imageInfo.mipLevels = 1;
            imageInfo.arrayLayers = 1;

            imageInfo.format = imageFormat;
            imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;

            imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

            imageInfo.usage =
                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                VK_IMAGE_USAGE_SAMPLED_BIT;

            imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
            imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            device.createImageWithInfo(
                imageInfo, 
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
                images[i], 
                imageAllocs[i]);

            VkImageViewCreateInfo viewInfo{};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = images[i];
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = imageFormat;
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(device.device(), &viewInfo, nullptr, &imageViews[i]) !=
                VK_SUCCESS) {
                throw std::runtime_error("failed to create texture image view!");
            }
        }
    }

    void RenderTarget::createRenderPass() {
        VkAttachmentDescription depthAttachment{};
        depthAttachment.format = findDepthFormat();
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription colorAttachment = {};
        colorAttachment.format = getImageFormat();
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        VkAttachmentReference colorAttachmentRef = {};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;

        VkSubpassDependency dependency = {};
        dependency.dstSubpass = 0;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.srcAccessMask = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        if (vkCreateRenderPass(device.device(), &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass!");
        }
    }

    void RenderTarget::createFramebuffers() {
        framebuffers.resize(imageCount());
        for (size_t i = 0; i < imageCount(); i++) {
            std::array<VkImageView, 2> attachments = { imageViews[i], depthImageViews[i] };

            VkExtent2D swapChainExtent = getExtent();
            VkFramebufferCreateInfo framebufferInfo = {};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = renderPass;
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = swapChainExtent.width;
            framebufferInfo.height = swapChainExtent.height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(
                device.device(),
                &framebufferInfo,
                nullptr,
                &framebuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
    }

    void RenderTarget::createDepthResources() {
        depthFormat = findDepthFormat();

        VkExtent2D swapChainExtent = getExtent();

        depthImages.resize(imageCount());
        depthImageAllocs.resize(imageCount());
        depthImageViews.resize(imageCount());

        for (int i = 0; i < depthImages.size(); i++) {
            VkImageCreateInfo imageInfo{};
            imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageInfo.imageType = VK_IMAGE_TYPE_2D;
            imageInfo.extent.width = swapChainExtent.width;
            imageInfo.extent.height = swapChainExtent.height;
            imageInfo.extent.depth = 1;
            imageInfo.mipLevels = 1;
            imageInfo.arrayLayers = 1;
            imageInfo.format = depthFormat;
            imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
            imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
            imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            imageInfo.flags = 0;

            device.createImageWithInfo(
                imageInfo,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                depthImages[i],
                depthImageAllocs[i]);

            VkImageViewCreateInfo viewInfo{};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = depthImages[i];
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = depthFormat;
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(device.device(), &viewInfo, nullptr, &depthImageViews[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create texture image view!");
            }
        }
    }

    VkFormat RenderTarget::findDepthFormat() {
        return device.findSupportedFormat(
            { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
              VK_IMAGE_TILING_OPTIMAL,
              VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    }
    
    void RenderTarget::beginRenderPass(FrameInfo& frame) {
        Texture::imageMemBarrier(
            images[frame.frameIndex], 
            imageFormat, 
            frame.commandBuffer, 
            VK_IMAGE_LAYOUT_UNDEFINED, 
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 
            1);

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass  = getRenderPass();
        renderPassInfo.framebuffer = getFrameBuffer(frame.frameIndex);

        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = getExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {clearColor.r, clearColor.g, clearColor.b, clearColor.a };
        clearValues[1].depthStencil = { 1.0f, 0 };
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(frame.commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(getExtent().width);
        viewport.height = static_cast<float>(getExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{ {0, 0}, getExtent() };
        vkCmdSetViewport(frame.commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(frame.commandBuffer, 0, 1, &scissor);
    }

    void RenderTarget::endRenderPass(FrameInfo& frame) {
        vkCmdEndRenderPass(frame.commandBuffer); 
    }
}