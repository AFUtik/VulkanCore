#include "gfx/backend/opengl/BackendGL.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "pch.hpp"

namespace gfx::gl 
{

BackendGL::BackendGL(Window* windowInstance) : Device(BackendType::OpenGL), windowInstance(windowInstance), screenFBO(*this)
{
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        exit(1);
    }
}

Handle<Image>         BackendGL::createImage (const ImageDesc& desc) 
{
    return images.Create(desc).Cast<Image>();
}

Handle<Buffer>        BackendGL::createBuffer(const BufferDesc& desc) 
{
    return buffers.Create(desc).Cast<Buffer>();
}

Handle<MeshDesc>      BackendGL::meshDescHandle(const MeshDesc& desc)
{
    return meshDescs.Create(desc);
}

Handle<Mesh>          BackendGL::createMesh  (Handle<MeshDesc>& desc) 
{
    return meshes.Create(*this, desc.Cast<MeshDescGL>()).Cast<Mesh>();
}

Handle<Shader>        BackendGL::createShader(const ShaderDesc& desc) 
{
    return shaders.Create(desc);
}

Handle<BindGroupLayout> BackendGL::createBindGroupLayout(const BindGroupLayoutDesc& desc)
{
    return bindGroupLayouts.Create(desc);
}

Handle<Framebuffer>   BackendGL::createFramebuffer     (const FramebufferDesc& desc)
{
    return framebuffers.Create(*this, desc);
}

Handle<RenderPass> BackendGL::createRenderPass      (const RenderPassDesc& desc)
{
    return renderPasses.Create(desc);
}

Handle<PipelineState> BackendGL::createPipelineState() 
{
    return pipelineStates.Create();
}

Handle<PipelineLayout> BackendGL::createPipelineLayout(const PipelineLayoutDesc& desc)
{
    return pipelineLayouts.Create(desc);
}

Handle<RenderPipeline> BackendGL::createRenderPipeline(const RenderPipelineDesc& desc) 
{
    return renderPipelines.Create(desc);
}

}
