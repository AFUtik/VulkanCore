#pragma once 

#include "renderers/PlanetRenderer.hpp"
#include "renderers/QuadTreeRenderer.hpp"

#include "vk/Pipeline.hpp"
#include "vk/Renderer.hpp"
#include "vk/RenderTarget.hpp"

#include "vk/Shader.hpp"

#define RENDER_WIDTH 640
#define RENDER_HEIGHT 320

struct Camera;

namespace myvk
{
    struct PipelineConfigInfo;
    struct BaseRenderSystem;
    struct Shader;
}

struct Renderer 
{
    Renderer();
    ~Renderer();

    myvk::Renderer vkRenderer;
    myvk::RenderTarget vkRenderTarget;

    myvk::ShaderManager shaderManager;
    myvk::Shader* baseShader;

    std::unique_ptr<myvk::BaseRenderSystem> baseRenderSystem;
    std::unique_ptr<myvk::BaseRenderSystem> screenRenderSystem;
    uint32_t mainPipeln, wireframePipeln;
    uint32_t screenPipeln; 

    PlanetRenderer planetRenderer;
    QuadTreeRenderer qtRenderer;

    BaseMesh vkMeshScreen;

    void render(Camera& camera);
private:
    void makeWireframeConfig(myvk::PipelineConfigInfo& config);

    void createVkMeshScreen();
};