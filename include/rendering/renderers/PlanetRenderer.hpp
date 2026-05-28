#pragma once

#include "game/GameContext.hpp"
#include "rendering/BaseMesh.hpp"
#include "rendering/RenderQueue.hpp"

#include "gfx/management/Handle.hpp"

namespace gfx
{
    struct Mesh;
}

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

    gfx::Handle<gfx::Mesh>    vkCircleMesh;
    std::vector<InstanceData> instances;
};