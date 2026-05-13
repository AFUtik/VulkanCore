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

    std::vector<myvk::Mesh> quads;
    std::vector<std::vector<myvk::InstanceData>> quadInstances;
};