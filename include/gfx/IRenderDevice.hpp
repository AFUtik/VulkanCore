#pragma once

#include <memory>
#include <cstdint>

#include "Handle.hpp"

struct Texture;

namespace gfx {

struct Material;
struct IMesh;

struct IMeshHandle;
struct IMaterialHandle;

struct ITexture       { virtual ~ITexture() = default; };
struct IBuffer        { virtual ~IBuffer()  = default; };
struct ICommandBuffer { virtual ~ICommandBuffer() = default;};

struct IRenderSystem
{
    virtual void render     (IMeshHandle*, IMaterialHandle*) {};
    virtual void renderBatch(IMeshHandle*, IMaterialHandle*, const void* instances, uint64_t instanceCount) {};

    virtual std::unique_ptr<IMaterialHandle> createMaterial(const Material&) {return {};};
    
};

struct IRenderDevice
{
    virtual ~IRenderDevice() = default;

    virtual Handle<IMesh> createMesh() {return {};}; 

    virtual std::unique_ptr<ITexture>        createTexture (const Texture&)   {return {};};
    virtual std::unique_ptr<IBuffer>         createBuffer  ()                 {return {};};
   
    virtual ICommandBuffer*                  beginFrame()                     {return nullptr;};
    virtual void                             endFrame()                       {};
};

} // namespace gfx