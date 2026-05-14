#pragma once

#include "game/GameContext.hpp"
#include "vk/Mesh.hpp"

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

    myvk::Mesh vkQuad;
    std::vector<myvk::InstanceData> quadInstances;
};