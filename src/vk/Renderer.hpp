#pragma once

#include "../window/Window.hpp"

#include "Device.hpp"
#include "Pipeline.hpp"
#include "Swapchain.hpp"
#include "Descriptors.hpp"

#include <memory>
#include <vector>
#include <cassert>

namespace myvk {
	class Renderer {
	public:
		Renderer(Window &window, Device &device);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;

		VkRenderPass getSwapChainRenderPass() { return swapchain->getRenderPass(); }
		bool isFrameInProgress() const { return isFrameStarted; }

		DescriptorPoolManager* getDescriptorPool() { return descriptorPoolManager.get(); }

		VkCommandBuffer getCurrentCommandBuffer() const { 
			assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
			return commandBuffers[currentFrameIndex]; 
		}

		int getFrameIndex() const { 
			assert(isFrameStarted && "Cannot get frame buffer index when frame not in progress");
			return currentFrameIndex; 
		}

		VkCommandBuffer beginFrame();
		void endFrame();

		void beginSwapChainRenderPassLowResolution(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPassLowResolution  (VkCommandBuffer commandBuffer);

		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass  (VkCommandBuffer commandBuffer);
	private:
		void createCommandBuffers();
		void recreateSwapChain();
		void freeCommandBuffers();

		Window& window;
		Device& device;

		std::unique_ptr<SwapChain> swapchain;
		std::unique_ptr<SwapChain> lowresSwapchain;

		std::vector<VkCommandBuffer> commandBuffers;

		std::unique_ptr<DescriptorPoolManager> descriptorPoolManager;

		uint32_t currentImageIndex;
		int currentFrameIndex = 0;
		bool isFrameStarted = false;
	};
}