#pragma once

#include <vulkan/vulkan.h>

#include <vector>
#include <string>

namespace vk {
	class Device;
	struct Shader;

	struct PipelineConfigInfo {
		VkPipelineViewportStateCreateInfo      viewportInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo   multisampleInfo;
		VkPipelineColorBlendAttachmentState    colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo    colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo  depthStencilInfo;
		std::vector<VkDynamicState>            dynamicStateEnables;
		VkPipelineDynamicStateCreateInfo       dynamicStateInfo;
		VkPipelineLayout                       pipelineLayout = nullptr;
		VkRenderPass                           renderPass = nullptr;
		uint32_t                               subpass = 0;

		const std::vector<VkVertexInputBindingDescription>&   bindings;
		const std::vector<VkVertexInputAttributeDescription>& attributes;
		Shader* shader = nullptr;
	};

	class Pipeline {
	public:
		Pipeline(const PipelineConfigInfo &configInfo);
		~Pipeline();

		void bind(VkCommandBuffer commandBuffer);

		Pipeline(const Pipeline&) = delete;
		Pipeline& operator=(const Pipeline&) = delete;

		static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo);
	private:
		static std::vector<char> readFile(const std::string& filepath);

		void createGraphicsPipeline(const PipelineConfigInfo& configInfo);
		void createShaderModule(const std::vector<uint32_t>& code, VkShaderModule *shaderModule);

		Device& device;
		VkPipeline graphicsPipeline;
		VkShaderModule vertShaderModule;
		VkShaderModule fragShaderModule;
	};
}
