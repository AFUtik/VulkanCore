#include "Model.hpp"

namespace myvk {

void Model::render(FrameInfo& frame, VkPipelineLayout pipelineLayout) {
    vkCmdPushConstants(
		frame.commandBuffer,
		pipelineLayout,
		VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
		0,
		sizeof(PushConstantData),
		&transform.model()
	);

    if(material) material->bind(frame.commandBuffer, pipelineLayout, frame.frameIndex);
    mesh->bind(frame.commandBuffer);
    mesh->draw(frame.commandBuffer);
}

}