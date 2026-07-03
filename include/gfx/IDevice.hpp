#pragma once

#include "Handle.hpp"
#include "pch.hpp"

namespace gfx 
{

enum BackendType
{
    OpenGL,
    Vulkan
};

struct BindGroupLayout;
struct Framebuffer;
struct RenderPass;
struct Shader;
struct Buffer;
struct Mesh;
struct Image;

struct PipelineState;
struct PipelineLayout;
struct RenderPipeline;

struct ShaderDesc;
struct BufferDesc;
struct MeshDesc;
struct ImageDesc;
struct RenderPassDesc;
struct PipelineLayoutDesc;
struct RenderPipelineDesc;
struct FramebufferDesc;
struct BindGroupLayoutDesc;

struct Device
{
    Device(BackendType type) : type(type) {};

    virtual Handle<Image>          createImage (const ImageDesc& desc)  = 0;
    virtual Handle<Buffer>         createBuffer(const BufferDesc& desc) = 0;

    virtual Handle<MeshDesc>       meshDescHandle(const MeshDesc& desc)   = 0;
    virtual Handle<Mesh>           createMesh    (Handle<MeshDesc>& desc) = 0;

    virtual Handle<Shader>         createShader(const ShaderDesc& desc) = 0;

    virtual Framebuffer&            getScreenFramebuffer  () = 0;
    virtual Handle<BindGroupLayout> createBindGroupLayout (const BindGroupLayoutDesc& desc) = 0;
    virtual Handle<Framebuffer>     createFramebuffer     (const FramebufferDesc& desc) = 0;
    virtual Handle<RenderPass>      createRenderPass      (const RenderPassDesc&  desc) = 0;
    virtual Handle<PipelineState>   createPipelineState   () = 0;
    virtual Handle<PipelineLayout>  createPipelineLayout  (const PipelineLayoutDesc& desc) = 0;
    virtual Handle<RenderPipeline>  createRenderPipeline  (const RenderPipelineDesc& decs) = 0;

    virtual ~Device() = default;

    inline BackendType getBackendType() const {return type;}
private:
    BackendType type;
};

}
