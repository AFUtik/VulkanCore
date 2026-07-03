#pragma once

#include "gfx/BindGroupLayout.hpp"
#include "gfx/IDevice.hpp"
#include "gfx/IShader.hpp"
#include "gfx/ResourceManager.hpp"
#include "gfx/PipelineState.hpp"
#include "PipelineLayout.hpp"

#include "Buffer.hpp"
#include "Mesh.hpp"
#include "Image.hpp"
#include "Shader.hpp"
#include "RenderPass.hpp"
#include "Framebuffer.hpp"
#include "RenderPipeline.hpp"
#include "BindGroupLayout.hpp"

struct Window;

namespace gfx::gl 
{

struct BackendGL : public Device
{
    BackendGL(Window* windowInstance);
    ~BackendGL() {};

    Handle<Image>          createImage (const ImageDesc& desc)  override;
    Handle<Buffer>         createBuffer(const BufferDesc& desc) override;

    Handle<MeshDesc>       meshDescHandle(const MeshDesc& desc) override;
    Handle<Mesh>           createMesh    (Handle<MeshDesc>& desc) override;

    Handle<Shader>         createShader(const ShaderDesc& desc) override;

    Handle<BindGroupLayout> createBindGroupLayout(const BindGroupLayoutDesc& desc) override;

    Handle<Framebuffer>    createFramebuffer     (const FramebufferDesc& desc) override;
    Handle<RenderPass>     createRenderPass      (const RenderPassDesc& desc)  override;
    Handle<PipelineState>  createPipelineState   ()  override;
    Handle<PipelineLayout> createPipelineLayout  (const PipelineLayoutDesc& desc) override;
    Handle<RenderPipeline> createRenderPipeline  (const RenderPipelineDesc& desc) override;

    inline Framebuffer&    getScreenFramebuffer() override
    {
        return screenFBO;
    }
private:
    Window*       windowInstance;
    FramebufferGL screenFBO;

    ResourceManager<ImageGL>       images;
    ResourceManager<BufferGL>      buffers;
    ResourceManager<MeshDescGL>    meshDescs;
    ResourceManager<MeshGL>        meshes;

    ResourceManager<ShaderGL>      shaders;
    ResourceManager<RenderPassGL>  renderPasses;

    ResourceManager<BindGroupLayoutGL> bindGroupLayouts;   
    ResourceManager<FramebufferGL>     framebuffers;
    
    // Big structures, block size was decreased
    ResourceManager<PipelineState,    64> pipelineStates; 
    ResourceManager<PipelineLayoutGL, 64> pipelineLayouts;
    ResourceManager<RenderPipelineGL, 64> renderPipelines;
};

}
