#include "rendering/Renderer.hpp"
#include "rendering/RenderState.hpp"
#include "rendering/systems/BaseRenderSystem.hpp"

#include "Camera.hpp"

#include "vk/Mesh.hpp"
#include "vk/Pipeline.hpp"
#include "vulkan/vulkan_core.h"

Renderer::Renderer()
    : vkRenderer(), 
      vkRenderTarget(vkRenderer.getSwapChain(), {RENDER_WIDTH, RENDER_HEIGHT}),
      planetRenderer(*this),
      qtRenderer(*this)
{
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
    
    vkScreenRenderSystem = std::make_unique<myvk::BaseRenderSystem>(vkRenderer, config);
    vkPlanetRenderSystem = std::make_unique<myvk::BaseRenderSystem>(vkRenderer, vkRenderTarget, config);

    makeWireframeConfig(config);
    vkWireframeRenderSystem = std::make_unique<myvk::BaseRenderSystem>(vkRenderer, vkRenderTarget, config);

    vkRenderTarget.createFramebufferTexture(vkScreenRenderSystem.get());
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

	vkMeshScreen.updateBuffers(std::as_bytes(std::span(vertices)), indices);
    
    #ifndef NDEBUG
    vkMeshScreen.addDebugInfo("ScreenMesh");
    #endif
}

void Renderer::render(Camera& camera)
{  
	vkRenderer.beginFrame();
    auto& frame = vkRenderer.frameInfo();
    
    RenderState state {
        frame, 
        camera.getProjview()
    };
			
    // BaseRenderSystem //
	vkRenderTarget.beginRenderPass(frame); // the begin of target's pass

    // Planet Renderer
    planetRenderer.submit(renderQueue);
    for(RenderBatch& batch : renderQueue.batchQueue) vkPlanetRenderSystem->render(state, batch);
    renderQueue.batchQueue.clear();
    vkPlanetRenderSystem->clearInstances();
    
    qtRenderer.submit(wireframeRenderQueue);
    for(RenderBatch& batch : wireframeRenderQueue.batchQueue) vkWireframeRenderSystem->render(state, batch);
    wireframeRenderQueue.batchQueue.clear();
    vkWireframeRenderSystem->clearInstances();

	vkRenderTarget.endRenderPass(frame); // the end of target's pass
    
	// SwapChain Render //
	vkRenderer.beginSwapChainRenderPass();
    
    state.projview = glm::mat4(1.0f);
    InstanceData instance{};
    RenderBatch screenBatch{
        &vkMeshScreen, 
        vkRenderTarget.getFramebufferTexture(frame),
        &instance,
        1};
	vkScreenRenderSystem->render(state, screenBatch);
	vkRenderer.endSwapChainRenderPass();
	vkRenderer.endFrame();
}

Renderer::~Renderer() = default;