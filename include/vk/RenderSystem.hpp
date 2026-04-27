#pragma once

#include <memory>
#include <vector>

struct VkPipelineLayout_T;
using VkPipelineLayout = VkPipelineLayout_T*;

struct VkRenderPass_T;
using VkRenderPass = VkRenderPass_T*;

struct VkDescriptorSetLayout_T;
using VkDescriptorSetLayout = VkDescriptorSetLayout_T*;

namespace myvk {
	class Device;

	class Renderer;
	class RenderTarget;

	class DescriptorPoolManager;
	class DescriptorSetLayout;

	struct PipelineConfigInfo;
	class  Pipeline;

	class RenderSystem {
	public:
		RenderSystem(Renderer& renderer);

		~RenderSystem();

		RenderSystem(const RenderSystem&) = delete;
		RenderSystem& operator=(const RenderSystem&) = delete;

		virtual DescriptorSetLayout* getMaterialSetLayout() = 0;

		inline VkPipelineLayout getPipelineLayout() {return pipelineLayout;}

		inline DescriptorPoolManager* getDescriptorPool() {return descriptorPool;}
	protected:
		void createPipelineLayout(const std::vector<VkDescriptorSetLayout>& layouts);
		void createPipeline(VkRenderPass renderPass, PipelineConfigInfo& pipelineConfig);
		
		DescriptorPoolManager* descriptorPool;

		std::unique_ptr<Pipeline> pipeline;
		VkPipelineLayout pipelineLayout;
		
		Device& device;
	};
}