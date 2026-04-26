#pragma once

#include "../vk/Mesh.hpp"
#include "../vk/Material.hpp"
#include "../vk/Pipeline.hpp"
#include "../vk/FrameInfo.hpp"
#include "../vk/Descriptors.hpp"

#include <memory>
#include <vector>
#include <array>

namespace myvk {
	class Renderer;
	class RenderTarget;

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

		Device& device = Device::instance();

		std::unique_ptr<Pipeline> pipeline;
		VkPipelineLayout pipelineLayout;

		DescriptorPoolManager* descriptorPool;
	};
}