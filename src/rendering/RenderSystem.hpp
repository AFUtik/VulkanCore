#pragma once

#include "../FreeList.hpp"
#include "../Camera.hpp"
#include "../model/MeshObject.hpp"

#include "RenderService.hpp"
#include "RenderComponent.hpp"

#include "../vk/GPUMesh.hpp"
#include "../vk/GPUMaterial.hpp"
#include "../vk/Pipeline.hpp"
#include "../vk/FrameInfo.hpp"

#include <memory>
#include <vector>

const std::string absolutePath = "C:\\cplusplus\\VulkanRender\\VulkanRender\\";

struct GlobalUbo {
	glm::mat4 projview{1.f};
};

namespace myvk {
	class RenderSystem;

	class RenderSystem {
	public:
		RenderSystem(Device& device, VkRenderPass renderPass, std::vector<VkDescriptorSetLayout> layouts, FrameInfo& frame);
		RenderSystem(Device& device, FrameInfo& frame) : device(device), frame(frame) {};
		~RenderSystem();

		RenderSystem(const RenderSystem&) = delete;
		RenderSystem& operator=(const RenderSystem&) = delete;

		virtual void render();
		RenderService* getRenderService() {return renderService.get();}
	protected:
		void createPipelineLayout(const std::vector<VkDescriptorSetLayout>& layouts);
		void createPipeline(VkRenderPass renderPass, PipelineConfigInfo& pipelineConfig);

		std::unique_ptr<RenderService> renderService;
		//std::vector<std::shared_ptr<>> renderComponents;

		Device& device;
		FrameInfo& frame;

		std::unique_ptr<Pipeline> pipeline;
		VkPipelineLayout pipelineLayout;

		DescriptorPoolManager* descriptorPool;
		std::unique_ptr<DescriptorSetLayout> setLayout;
		std::unique_ptr<DescriptorSetLayout> materialSetLayout;
		std::vector<DescriptorSetData> descriptorSets;
		std::vector<VkDescriptorSetLayout> layouts;
		std::vector<std::unique_ptr<Buffer>> uniforms;
	};
}