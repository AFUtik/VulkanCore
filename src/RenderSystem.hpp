#pragma once

#include "FreeList.hpp"
#include "Pipeline.hpp"
#include "FrameInfo.hpp"
#include "Camera.hpp"

#include "model/GPUMesh.hpp"
#include "model/GPUMaterial.hpp"
#include "model/Model.hpp"

#include <memory>
#include <vector>

const std::string absolutePath = "C:\\cplusplus\\VulkanRender\\VulkanRender\\";

struct GlobalUbo {
	glm::mat4 projview{1.f};
};

namespace myvk {
	struct GPUModel {
		std::shared_ptr<GPUMaterial> material;
		std::shared_ptr<GPUMesh> mesh;
	};

	class ObjectRenderer {
	public:
		RenderSystem* renderSystem;

		virtual void buildDrawList() = 0;
	};

	class RenderSystem {
	public:
		RenderSystem(Device& device, VkRenderPass renderPass, std::vector<VkDescriptorSetLayout> layouts, FrameInfo& frame);
		RenderSystem(Device& device, FrameInfo& frame) : device(device), frame(frame) {};
		~RenderSystem();

		RenderSystem(const RenderSystem&) = delete;
		RenderSystem& operator=(const RenderSystem&) = delete;

		virtual void render();
		void registerRenderer(std::shared_ptr<ObjectRenderer> renderer);

		inline void addToDrawList(Model* model) {drawList.push_back(model);}

		void createModel(Model* model);

		/*void deleteModel(Model* model);*/
	protected:
		void createPipelineLayout(const std::vector<VkDescriptorSetLayout>& layouts);
		void createPipeline(VkRenderPass renderPass, PipelineConfigInfo& pipelineConfig);
		void createEmptyMaterial();

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

		std::vector<Model*> drawList;
		std::vector<std::shared_ptr<ObjectRenderer>> renderers;
		FreeList<std::unique_ptr<GPUModel>>  modelsFreelist;
		FreeList<std::weak_ptr<GPUMaterial>> materialsFreelist;
		FreeList<std::weak_ptr<GPUMesh>>     meshesFreelist;

		std::shared_ptr<GPUMaterial> emptyMaterial;
	};
}