#pragma once

#include <memory>
#include <vector>
#include <cstdint>

struct VkPipelineLayout_T;
using VkPipelineLayout = VkPipelineLayout_T*;

struct VkRenderPass_T;
using VkRenderPass = VkRenderPass_T*;

struct VkDescriptorSetLayout_T;
using VkDescriptorSetLayout = VkDescriptorSetLayout_T*;

namespace vk {
	class Device;

	class Renderer;
	class RenderTarget;

	class DescriptorPoolManager;
	class DescriptorSetLayout;

	struct PipelineConfigInfo;
	class  Pipeline;

	struct RenderTarget;

	class RenderSystem {
	public:
		RenderSystem(Renderer& renderer);

		~RenderSystem();

		RenderSystem(const RenderSystem&) = delete;
		RenderSystem& operator=(const RenderSystem&) = delete;

		virtual DescriptorSetLayout* getMaterialSetLayout() = 0;

		inline VkPipelineLayout getPipelineLayout() {return pipelineLayout;}

		inline DescriptorPoolManager* getDescriptorPool() {return descriptorPool;}
		
		void addPipeline(VkRenderPass renderPass, PipelineConfigInfo& pipelineConfig, uint32_t& pipelineId);
	protected:
		void createPipelineLayout(const std::vector<VkDescriptorSetLayout>& layouts);
		
		VkPipelineLayout pipelineLayout;
		
		DescriptorPoolManager* descriptorPool;
		std::vector<std::unique_ptr<Pipeline>> pipelines;
		
		Device& device;
	};
}