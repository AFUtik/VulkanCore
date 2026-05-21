#include "rendering/Renderer.hpp"
#include "rendering/RenderState.hpp"
#include "rendering/systems/BaseRenderSystem.hpp"

#include "Camera.hpp"

#include "vk/Pipeline.hpp"
#include "vulkan/vulkan_core.h"

Renderer::Renderer()
    : vkRenderer(), 
      vkRenderTarget(vkRenderer.getSwapChain(), {RENDER_WIDTH, RENDER_HEIGHT}),
      planetRenderer(*this),
      qtRenderer(*this)
{
    baseRenderSystem   = std::make_unique<myvk::BaseRenderSystem>(vkRenderer);
    screenRenderSystem = std::make_unique<myvk::BaseRenderSystem>(vkRenderer);

    baseShader = shaderManager.loadShader(
    "/home/afutik/cplusplus/VulkanCore/resources/shaders/shader.vert", 
    "/home/afutik/cplusplus/VulkanCore/resources/shaders/shader.frag",
    "BaseShader");

    myvk::PipelineConfigInfo config{
        .bindings   = myvk::BaseRenderSystem::getBindingDescriptions(),
        .attributes = myvk::BaseRenderSystem::getAttributeDescriptions(),
        .shader     = baseShader
    };
    myvk::Pipeline::defaultPipelineConfigInfo(config);

    baseRenderSystem->addPipeline(
        vkRenderTarget.getRenderPass(), 
        config, 
        mainPipeln);

    screenRenderSystem->addPipeline(
        vkRenderer.getSwapChainRenderPass(), 
        config,
        screenPipeln 
    );

    makeWireframeConfig(config);
    baseRenderSystem->addPipeline(
        vkRenderTarget.getRenderPass(), 
        config, 
        wireframePipeln);

    vkRenderTarget.createFramebufferTexture(baseRenderSystem.get());
    createVkMeshScreen();
}

void Renderer::makeWireframeConfig(myvk::PipelineConfigInfo& config)
{
    config.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    config.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
}

void Renderer::createVkMeshScreen()
{ 
    std::vector<Vertex> vertices; 
    std::vector<u32> indices; 

	vertices.push_back({1.0f,   1.0f, 0.0f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f});
	vertices.push_back({1.0f,  -1.0f, 0.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f});
	vertices.push_back({-1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f});
	vertices.push_back({-1.0f,  1.0f, 0.0f,  0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f});

	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(2);
	indices.push_back(3);
	indices.push_back(0);

	vkMeshScreen.updateVertexBuffer(vertices.data(), vertices.size());
    vkMeshScreen.updateIndexBuffer(indices.data(), indices.size());
    
    #ifndef NDEBUG
    vkMeshScreen.addDebugInfo("ScreenMesh");
    #endif
}

void Renderer::render(Camera& camera)
{  
	vkRenderer.beginFrame();
    auto& frame = vkRenderer.frameInfo();
    
    RenderState state {
        .frame=frame, 
        .projview=camera.getProjview(),
        .rsystem=0,
        .pipeline=0
    };
    RenderQueue& renderQueue = baseRenderSystem->getRenderQueue();

	vkRenderTarget.beginRenderPass(frame);

    planetRenderer.submit(renderQueue);
    state.pipeline = mainPipeln;
    baseRenderSystem->flushRenderQueue(state);

    qtRenderer.submit(renderQueue);
    state.pipeline = wireframePipeln;
    baseRenderSystem->flushRenderQueue(state);

    baseRenderSystem->clearQueue();

	vkRenderTarget.endRenderPass(frame);



	vkRenderer.beginSwapChainRenderPass();
    
    state.projview = glm::mat4(1.0f);
    InstanceData instance{};
    RenderBatch screenBatch{
        &vkMeshScreen, 
        vkRenderTarget.getFramebufferTexture(frame),
        &instance,
        1};
    state.pipeline = screenPipeln;
    screenRenderSystem->getRenderQueue().batchQueue.push_back(screenBatch);
	screenRenderSystem->flushRenderQueue(state);
    screenRenderSystem->clearQueue();
    
	vkRenderer.endSwapChainRenderPass();
	vkRenderer.endFrame();
}

Renderer::~Renderer() = default;