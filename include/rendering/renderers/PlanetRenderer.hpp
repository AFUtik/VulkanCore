#pragma once

#include "vk/Mesh.hpp"

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

    myvk::Mesh vkCircleMesh;

    std::vector<myvk::InstanceData> instances;
};