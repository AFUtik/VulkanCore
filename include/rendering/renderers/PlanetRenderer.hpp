#pragma once

#include "game/GameContext.hpp"
#include "rendering/BaseMesh.hpp"
#include "rendering/RenderQueue.hpp"

#include "gfx/Handle.hpp"
#include "gfx/IMesh.hpp"

struct Renderer;
struct RenderQueue;
struct PlanetComponentManager;

struct PlanetRenderer
{
    PlanetRenderer(Renderer& renderer);
    
    void submit(RenderQueue& queue);
    void calculateInstances();
private:
    Renderer& renderer;
    
    PlanetComponentManager& pcm;

    gfx::Handle<gfx::IMesh>     vkCircleMesh;
    std::vector<InstanceData> instances;
};