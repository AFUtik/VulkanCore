#include "RenderSystem.hpp"
#include "FrameInfo.hpp"
#include "Pipeline.hpp"
#include "model/GPUTexture.hpp"
#include "model/Mesh.hpp"

#include <stdexcept>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace myvk {

RenderSystem::RenderSystem(Device &device, VkRenderPass renderPass, std::vector<VkDescriptorSetLayout> layouts, FrameInfo& frame) : device(device), frame(frame) {
	createPipelineLayout(layouts);
	createEmptyMaterial();

	PipelineConfigInfo config{};
	Pipeline::defaultPipelineConfigInfo(config);

	createPipeline(renderPass, config);
}

RenderSystem::~RenderSystem() {
	vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
}

void RenderSystem::registerRenderer(std::shared_ptr<ObjectRenderer> renderer) {
	renderer->renderSystem = this;
	renderers.push_back(renderer);
}

void RenderSystem::createModel(Model* model) {
	std::unique_ptr<GPUModel> gpuModel = std::make_unique<GPUModel>();
	if(model->modelId < 0) {
		if(model->meshHandle < 0 && model->meshInstance) {
			auto mesh = std::make_shared<GPUMesh>(device, *model->meshInstance.get());
			uint32_t handle = meshesFreelist.create(mesh);
			model->meshHandle = handle;
			gpuModel->mesh = mesh;
		}

		if(!model->albedo) {
			gpuModel->material = emptyMaterial; 
		}
		else if(model->materialHandle < 0) {
			auto texture  = std::make_shared<GPUTexture>(device, model->albedo.get());
			auto material = std::make_shared<GPUMaterial>(*descriptorPool, *materialSetLayout, texture);
			uint32_t handle = materialsFreelist.create(material);
			model->materialHandle = handle;
		}
	}
	model->modelId = modelsFreelist.create(std::move(gpuModel));
}

void RenderSystem::createEmptyMaterial() {
	std::unique_ptr<uint8_t[]> whitePixel = std::make_unique<uint8_t[]>(4);
	whitePixel[0] = 255;
	whitePixel[1] = 255;
	whitePixel[2] = 255;
	whitePixel[3] = 255;

	std::shared_ptr<Texture2D> texture     = std::make_shared<Texture2D>(std::move(whitePixel), 1, 1, TextureChannels::RGBA);
	std::shared_ptr<GPUTexture> gpuTexture = std::make_shared<GPUTexture>(device, texture.get());
	emptyMaterial = std::make_unique<GPUMaterial>(*descriptorPool, *materialSetLayout, gpuTexture);
}

void RenderSystem::createPipelineLayout(const std::vector<VkDescriptorSetLayout>& layouts_) {
	VkPushConstantRange pushConstantRange{};
	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(PushConstantData);

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(layouts_.size());
	pipelineLayoutInfo.pSetLayouts = layouts_.data();
	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
	if (vkCreatePipelineLayout(device.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
		VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}
}

void RenderSystem::createPipeline(VkRenderPass renderPass ,PipelineConfigInfo& pipelineConfig) {
	assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

	pipelineConfig.renderPass = renderPass;
	pipelineConfig.pipelineLayout = pipelineLayout;
	pipeline = std::make_unique<Pipeline>(
		device,
		absolutePath + "resources/shaders/shader.vert.spv",
		absolutePath + "resources/shaders/shader.frag.spv",
		pipelineConfig);
}

void RenderSystem::render() {
	for(auto& renderer : renderers) renderer->buildDrawList();

	pipeline->bind(frame.commandBuffer);
	vkCmdBindDescriptorSets(
		frame.commandBuffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		pipelineLayout,
		0,
		1,
		&descriptorSets[frame.frameIndex].set,
		0, nullptr
	);
	for(Model* model : drawList) {
		if(model->modelId < 0) createModel(model);

		auto& gpuModel = modelsFreelist[model->modelId];
		vkCmdPushConstants(
			frame.commandBuffer,
			pipelineLayout,
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
			0,
			sizeof(PushConstantData),
			&model->transform.model()
		);

		gpuModel->material->bind(frame.commandBuffer, pipelineLayout, frame.frameIndex);
		gpuModel->mesh->bind(frame.commandBuffer);
		gpuModel->mesh->draw(frame.commandBuffer);
	}
	drawList.clear();
}

}