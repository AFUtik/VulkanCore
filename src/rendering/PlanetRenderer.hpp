#pragma once

#include "RenderState.hpp"

#include "vk/Mesh.hpp"

struct Renderer;
struct RenderQueue;

struct PlanetRenderer
{
    PlanetRenderer(Renderer& renderer);
    
    void submit(RenderQueue& queue);
private:
    Renderer& renderer;
    myvk::Mesh vkCircleMesh;
};