#pragma once

#include "vk/Mesh.hpp"
#include "rendering/BaseMesh.hpp"

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

    BaseMesh vkCircleMesh;
    std::vector<InstanceData> instances;
};