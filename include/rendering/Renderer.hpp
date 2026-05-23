#pragma once 

#include "renderers/PlanetRenderer.hpp"
#include "renderers/QuadTreeRenderer.hpp"

#include "gfx/vk/Pipeline.hpp"
#include "gfx/vk/Renderer.hpp"
#include "gfx/vk/RenderTarget.hpp"
#include "gfx/vk/Shader.hpp"
#include "gfx/ResourceManager.hpp"

#define RENDER_WIDTH 640
#define RENDER_HEIGHT 320

struct Camera;

namespace vk
{
    struct PipelineConfigInfo;
    struct BaseRenderSystem;
}

struct Renderer 
{
    Renderer();
    ~Renderer();

    vk::Renderer vkRenderer;
    vk::RenderTarget vkRenderTarget;

    vk::ShaderManager shaderManager;
    vk::Shader* baseShader;

    std::unique_ptr<vk::BaseRenderSystem> baseRenderSystem;
    std::unique_ptr<vk::BaseRenderSystem> screenRenderSystem;
    uint32_t mainPipeln, wireframePipeln;
    uint32_t screenPipeln; 

    PlanetRenderer planetRenderer;
    QuadTreeRenderer qtRenderer;

    BaseMesh vkMeshScreen;

    void render(Camera& camera);
private:
    void makeWireframeConfig(vk::PipelineConfigInfo& config);

    void createVkMeshScreen();
};

extern gfx::ResourceManager meshManager;