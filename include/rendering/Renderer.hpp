#pragma once 

#include "renderers/PlanetRenderer.hpp"
#include "systems/BaseRenderSystem.hpp"
#include "systems/WireframeRenderSystem.hpp"

#include "vk/Renderer.hpp"
#include "vk/RenderTarget.hpp"

#include "RenderState.hpp"
#include "RenderQueue.hpp"

#define RENDER_WIDTH 320
#define RENDER_HEIGHT 180

struct Camera;

struct Renderer 
{
    Renderer();
    ~Renderer() {};

    myvk::Renderer vkRenderer;
    myvk::RenderTarget vkRenderTarget;

    myvk::BaseRenderSystem vkScreenRenderSystem;
    myvk::BaseRenderSystem vkPlanetRenderSystem; 
    myvk::WireframeRenderSystem vkWireframeRenderSystem;

    PlanetRenderer planetRenderer;

    RenderQueue renderQueue;
    RenderQueue wireframeRenderQueue;

    myvk::Mesh vkMeshScreen;

    void render(Camera& camera);

    RenderQueue& getRenderQueue() {return renderQueue;}
    RenderQueue& getWireframeRenderQueue() {return wireframeRenderQueue;}
private:
    void createVkMeshScreen();
};