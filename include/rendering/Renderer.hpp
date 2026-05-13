#pragma once 

#include "renderers/PlanetRenderer.hpp"
#include "renderers/QuadTreeRenderer.hpp"

#include "vk/Pipeline.hpp"
#include "vk/Renderer.hpp"
#include "vk/RenderTarget.hpp"

#include "RenderQueue.hpp"

#define RENDER_WIDTH 320
#define RENDER_HEIGHT 180

struct Camera;

namespace myvk
{
    struct PipelineConfigInfo;
    struct BaseRenderSystem;
}

struct Renderer 
{
    Renderer();
    ~Renderer();

    myvk::Renderer vkRenderer;
    myvk::RenderTarget vkRenderTarget;

    std::unique_ptr<myvk::BaseRenderSystem> vkScreenRenderSystem;
    std::unique_ptr<myvk::BaseRenderSystem> vkPlanetRenderSystem; 
    std::unique_ptr<myvk::BaseRenderSystem> vkWireframeRenderSystem;

    PlanetRenderer planetRenderer;
    QuadTreeRenderer qtRenderer;

    RenderQueue renderQueue;
    RenderQueue wireframeRenderQueue;

    myvk::Mesh vkMeshScreen;

    void render(Camera& camera);

    RenderQueue& getRenderQueue() {return renderQueue;}
    RenderQueue& getWireframeRenderQueue() {return wireframeRenderQueue;}
private:
    void makeWireframeConfig(myvk::PipelineConfigInfo& config);

    void createVkMeshScreen();
};