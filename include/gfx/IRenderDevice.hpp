#pragma once

#include <memory>
#include <cstdint>

#include "Handle.hpp"

struct Texture;

namespace gfx {

struct Material;
struct Mesh;

struct IMeshHandle;
struct IMaterialHandle;

struct ITexture       { virtual ~ITexture() = default; };
struct IBuffer        { virtual ~IBuffer()  = default; };
struct ICommandBuffer { virtual ~ICommandBuffer() = default;};

struct IRenderSystem
{
    virtual void render     (IMeshHandle*, IMaterialHandle*);
    virtual void renderBatch(IMeshHandle*, IMaterialHandle*, const void* instances, uint64_t instanceCount);

    virtual std::unique_ptr<IMaterialHandle> createMaterial(const Material&)  = 0;
    virtual std::unique_ptr<IMeshHandle>     createMesh(const Mesh&)         = 0; 
};

struct IRenderDevice
{
    virtual ~IRenderDevice() = default;

    virtual std::unique_ptr<ITexture>        createTexture (const Texture&)   = 0;
    virtual std::unique_ptr<IBuffer>         createBuffer  ()                 = 0;
   
    virtual ICommandBuffer*                  beginFrame()                     = 0;
    virtual void                             endFrame()                       = 0;
};

} // namespace gfx