#include "rendering/Renderer.hpp"
#include "rendering/RenderQueue.hpp"
#include "rendering/renderers/PlanetRenderer.hpp"

#include "Camera.hpp"
#include "model/Mesh.hpp"
#include "vk/Mesh.hpp"

Renderer::Renderer()
    : vkRenderer(), 
      vkRenderTarget(vkRenderer.getSwapChain(), {RENDER_WIDTH, RENDER_HEIGHT}),
      vkScreenRenderSystem(vkRenderer),
      vkPlanetRenderSystem(vkRenderer, vkRenderTarget),
      vkWireframeRenderSystem(vkRenderer, vkRenderTarget),
      planetRenderer(*this)
{
    vkRenderTarget.createFramebufferTexture(&vkScreenRenderSystem);
    createVkMeshScreen();
}

void Renderer::createVkMeshScreen()
{ 
    Mesh screenMesh;
	screenMesh.vertices.push_back({1.0f,  1.0f, 0.0f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f});
	screenMesh.vertices.push_back({1.0f,  -1.0f, 0.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f});
	screenMesh.vertices.push_back({-1.0f,  -1.0f, 0.0f,  0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f});
	screenMesh.vertices.push_back({-1.0f,  1.0f, 0.0f,  0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f});

	screenMesh.indices.push_back(0);
	screenMesh.indices.push_back(1);
	screenMesh.indices.push_back(2);
	screenMesh.indices.push_back(2);
	screenMesh.indices.push_back(3);
	screenMesh.indices.push_back(0);
	vkMeshScreen.updateBuffers(screenMesh.vertices, screenMesh.indices);
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
	vkRenderTarget.beginRenderPass(frame);

    planetRenderer.submit(renderQueue);
    for(RenderBatch& batch : renderQueue.batchQueue) vkPlanetRenderSystem.render(state, batch);
    
    renderQueue.batchQueue.clear();
    
	vkRenderTarget.endRenderPass(frame);
    
	// SwapChain Render //
	vkRenderer.beginSwapChainRenderPass();
    
    state.projview = glm::mat4(1.0f);
    myvk::InstanceData instance{};
    RenderBatch screenBatch{
        &vkMeshScreen, 
        vkRenderTarget.getFramebufferTexture(frame),
        &instance,
        1};
	vkScreenRenderSystem.render(state, screenBatch);
	vkRenderer.endSwapChainRenderPass();
	vkRenderer.endFrame();
}