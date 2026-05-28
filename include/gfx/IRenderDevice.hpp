#pragma once

#include <cstdint>
#include <vector>

struct Texture;

namespace gfx {

enum ImageFilter
{
    LINEAR,
    NEAREST,
    LINEAR_MIPMAP,
    NEAREST_MIPMAP
};
struct Image
{
    virtual void writeToImage(
        const uint8_t* pixels,
        uint32_t width,
        uint32_t height,
        uint32_t channels
    ) = 0;
    
    virtual void setImageFilter(ImageFilter filter) = 0;

    virtual ~Image() = default;
};

struct Buffer
{
    virtual void writeToBuffer(const void* data, uint64_t size, uint64_t offset) = 0;
    virtual uint64_t getBufferSize() const = 0;

    virtual ~Buffer() = default;
};

struct Mesh
{
    virtual void updateVertexBuffer   (const void* vertices, uint64_t size)  = 0;
    virtual void updateIndexBuffer    (const void* indices, uint64_t size)   = 0;
    virtual void updateInstanceBuffer (const void* instances, uint64_t size) = 0;

    virtual const Buffer* getVertexBuffer() const   = 0;
    virtual const Buffer* getIndexBuffer() const    = 0;
    virtual const Buffer* getInstanceBuffer() const = 0;

    virtual ~Mesh() = default;
};

struct IRenderDevice
{
    virtual ~IRenderDevice() = default;
};

} // namespace gfx