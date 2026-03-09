#include "UIRenderSystem.hpp"
#include "Descriptors.hpp"

#include "FrameInfo.hpp"
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
	FrameInfo& frame) : RenderSystem(device, frame)
{
	descriptorPool = pool;

	createUILayouts();
	createEmptyMaterial();
    createPipelineLayout(layouts);

	PipelineConfigInfo config{};
	Pipeline::defaultPipelineConfigInfo(config);
	config.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;

	config.depthStencilInfo.depthTestEnable = VK_TRUE;
	config.depthStencilInfo.depthWriteEnable = VK_TRUE;
	config.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_ALWAYS;

	config.colorBlendAttachment.blendEnable = VK_TRUE;
	config.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	config.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	config.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	createPipeline(renderPass, config);
}

void UIRenderSystem::createUILayouts() {
	uniforms.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
	for(int i = 0; i < uniforms.size(); i++) {
		uniforms[i] = std::make_unique<Buffer>(
			device, 
			sizeof(UBO),
			1,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, 
			VMA_MEMORY_USAGE_CPU_TO_GPU);
		uniforms[i]->map();
	}

	setLayout = DescriptorSetLayout::Builder(device)
		.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
		.build();
	layouts.push_back(setLayout->getDescriptorSetLayout());

	materialSetLayout = DescriptorSetLayout::Builder(device)
		.addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		.build();
	layouts.push_back(materialSetLayout->getDescriptorSetLayout());

	descriptorSets.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
	for(int i = 0; i < descriptorSets.size(); i++) {
		auto bufferInfo = uniforms[i]->descriptorInfo();
		DescriptorWriter(*setLayout, *descriptorPool)
			.writeBuffer(0, &bufferInfo)
			.build(descriptorSets[i]);
	}

	uniforms[0]->writeToBuffer(&frame.camera.getProjviewOrtho());
	uniforms[0]->flush();

	uniforms[1]->writeToBuffer(&frame.camera.getProjviewOrtho());
	uniforms[1]->flush();
}

/*
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
*/

}