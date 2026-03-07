#include "UIRenderSystem.hpp"
#include "Descriptors.hpp"

#include "glm/ext/matrix_float4x4.hpp"

#include <locale>
#include <codecvt>
#include "Fonts.hpp"

namespace myvk {

struct UBO {
	glm::mat4 projview;
};

UIRenderSystem::UIRenderSystem(
	Device &device, 
	VkRenderPass renderPass, 
	DescriptorPoolManager* pool, 
	GUIContext* context,
	Camera* camera) : RenderSystem(device), guiRender(std::make_unique<GUIRender>(context)), descriptorPool(pool)
{
	createUILayouts(camera);
	createEmptyMaterial();
	createUIFont();
	
	guiRender->fetchContext(device);
	
    createPipelineLayout({uiSetLayout->getDescriptorSetLayout(), uiMaterialSetLayout->getDescriptorSetLayout()});

	PipelineConfigInfo config{};
	Pipeline::defaultPipelineConfigInfo(config);
	config.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;

	config.depthStencilInfo.depthTestEnable  = VK_FALSE;
	config.depthStencilInfo.depthWriteEnable = VK_FALSE;

	config.colorBlendAttachment.blendEnable = VK_TRUE;
	config.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	config.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	config.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	createPipeline(renderPass, config);
}

void UIRenderSystem::createUILayouts(Camera* camera) {
	uiUniform.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
	for(int i = 0; i < uiUniform.size(); i++) {
		uiUniform[i] = std::make_unique<Buffer>(
			device, 
			sizeof(UBO),
			1,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, 
			VMA_MEMORY_USAGE_CPU_TO_GPU);
		uiUniform[i]->map();
	}

	uiSetLayout = DescriptorSetLayout::Builder(device)
		.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
		.build();
	uiLayouts.push_back(uiSetLayout->getDescriptorSetLayout());

	uiMaterialSetLayout = DescriptorSetLayout::Builder(device)
		.addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		.build();
	uiLayouts.push_back(uiMaterialSetLayout->getDescriptorSetLayout());

	uiDescriptorSets.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
	for(int i = 0; i < uiDescriptorSets.size(); i++) {
		auto bufferInfo = uiUniform[i]->descriptorInfo();
		DescriptorWriter(*uiSetLayout, *descriptorPool)
			.writeBuffer(0, &bufferInfo)
			.build(uiDescriptorSets[i]);
	}

	uiUniform[0]->writeToBuffer(&camera->getProjviewOrtho());
	uiUniform[0]->flush();

	uiUniform[1]->writeToBuffer(&camera->getProjviewOrtho());
	uiUniform[1]->flush();
}

void UIRenderSystem::createUIFont() {
	std::string utf8text = "Простой текст в Vulkan API. ";
	std::u32string u32text = std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{}.from_bytes(utf8text);

	FontSample* sample = fontHandler.getSample("Times");
	curFont = std::make_unique<Font>(sample);
	curText = std::make_unique<Text>(curFont.get(), u32text);

	// TEXT MESH CREATING //
	auto textModel = std::make_shared<Model>();
	textModel->transform.translate({100.0, 900.0, 0.0});
	TextMesh textMesh(curText.get());
	
	std::shared_ptr<GPUTexture> bitmapFontGPU = std::make_shared<GPUTexture>(device, curText->getFont()->fontData.bitmap.get(), TextureFilter::Nearest);
	textModel->material = std::make_shared<GPUMaterial>(*descriptorPool, *uiMaterialSetLayout, bitmapFontGPU);
	textModel->mesh = std::make_shared<GPUMesh>(device, textMesh, GPUMeshBufferFlags::CreateWithReserve);

	models.push_back(textModel);
}

void UIRenderSystem::createEmptyMaterial() {
	std::unique_ptr<uint8_t[]> whitePixel = std::make_unique<uint8_t[]>(4);
	whitePixel[0] = 255;
	whitePixel[1] = 255;
	whitePixel[2] = 255;
	whitePixel[3] = 255;

	std::shared_ptr<Texture2D> texture     = std::make_shared<Texture2D>(std::move(whitePixel), 1, 1, TextureChannels::RGBA);
	std::shared_ptr<GPUTexture> gpuTexture = std::make_shared<GPUTexture>(device, texture.get());
	emptyMaterial = std::make_unique<GPUMaterial>(*descriptorPool, *uiMaterialSetLayout, gpuTexture);
}

void UIRenderSystem::render(FrameInfo& frame) {
	pipeline->bind(frame.commandBuffer);
	vkCmdBindDescriptorSets(
		frame.commandBuffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		pipelineLayout,
		0,
		1,
		&uiDescriptorSets[frame.frameIndex].set,
		0, nullptr
	);
	for(auto& window : guiRender->windowToRender) {
		for(auto& contentModel : window->models) {
			emptyMaterial->bind(frame.commandBuffer, pipelineLayout, frame.frameIndex);
			contentModel.model->render(frame, pipelineLayout);
		}
	}
}

}