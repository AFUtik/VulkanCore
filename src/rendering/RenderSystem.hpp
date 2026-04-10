#pragma once

#include "../FreeList.hpp"
#include "../Camera.hpp"
#include "../model/MeshObject.hpp"

#include "../vk/Mesh.hpp"
#include "../vk/Material.hpp"
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

		virtual void render(
			Mesh* mesh, 
			Material* mat, 
			const glm::mat4& model,
			Camera* camera);

		inline DescriptorSetLayout* getMaterialSetLayout() {
			return materialSetLayout.get();
		}
	protected:
		void createPipelineLayout(const std::vector<VkDescriptorSetLayout>& layouts);
		void createPipeline(VkRenderPass renderPass, PipelineConfigInfo& pipelineConfig);

		Device& device;
		FrameInfo& frame;

		std::unique_ptr<Pipeline> pipeline;
		VkPipelineLayout pipelineLayout;

		GlobalUbo ubo{};

		DescriptorPoolManager* descriptorPool;
		std::unique_ptr<DescriptorSetLayout> setLayout;
		std::unique_ptr<DescriptorSetLayout> materialSetLayout;
		std::vector<DescriptorSetData> descriptorSets;
		std::vector<VkDescriptorSetLayout> layouts;
		std::vector<std::unique_ptr<Buffer>> uniforms;
	};
}