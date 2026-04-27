#pragma once 

#include "systems/BaseRenderSystem.hpp"

#include "PlanetRenderer.hpp"

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

    PlanetRenderer planetRenderer;

    RenderQueue renderQueue;

    myvk::Mesh vkMeshScreen;

    void render(Camera& camera);
private:
    void createVkMeshScreen();
};