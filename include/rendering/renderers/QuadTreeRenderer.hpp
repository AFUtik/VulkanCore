#pragma once

#include "game/GameContext.hpp"
#include "rendering/RenderQueue.hpp"

#include "gfx/Handle.hpp"

#include <vector>

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
    gfx::Handle<BaseMesh> vkQuad;
};