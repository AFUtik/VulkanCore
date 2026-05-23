#pragma once

#include <cstdint>

namespace gfx 
{

struct IMeshHandle
{
    virtual void updateVertexBuffer   (const void* vertices, uint64_t size);
    virtual void updateIndexBuffer    (const void* indices, uint64_t size);
    virtual void updateInstanceBuffer (const void* instances, uint64_t size);
    
    virtual void bind(void* commandBuffer) const = 0;
};

}