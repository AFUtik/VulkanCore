#pragma once

#include "game/GameContext.hpp"
#include "rendering/RenderQueue.hpp"

#include "gfx/management/Handle.hpp"

#include <vector>

namespace gfx 
{
    struct Mesh;
}

struct Renderer;
struct RenderQueue;

struct QuadTreeRenderer
{
    QuadTreeRenderer(Renderer& renderer);

    void submit(RenderQueue& queue);
private:
    Renderer& renderer;
    const GameContext::QT& quadTree;
    
    std::vector<InstanceData> quadInstances;
    gfx::Handle<gfx::Mesh> vkQuad;
};