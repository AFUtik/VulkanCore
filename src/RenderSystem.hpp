#pragma once

#include "Pipeline.hpp"
#include "FrameInfo.hpp"
#include "Camera.hpp"

#include "model/Model.hpp"

#include <memory>
#include <vector>

const std::string absolutePath = "C:\\cplusplus\\VulkanRender\\VulkanRender\\";

namespace myvk {
	class RenderSystem {
	public:
		RenderSystem(Device& device, VkRenderPass renderPass, std::vector<VkDescriptorSetLayout> layouts);
		RenderSystem(Device& device) : device(device) {};
		~RenderSystem();

		RenderSystem(const RenderSystem&) = delete;
		RenderSystem& operator=(const RenderSystem&) = delete;

		virtual void render(FrameInfo& frame);
		void addModel(std::shared_ptr<Model> model);
	protected:
		void createPipelineLayout(const std::vector<VkDescriptorSetLayout>& layouts);
		void createPipeline(VkRenderPass renderPass, PipelineConfigInfo& pipelineConfig);

		std::vector<std::shared_ptr<Model>> models;

		Device& device;

		std::unique_ptr<Pipeline> pipeline;
		VkPipelineLayout pipelineLayout;
	};
}