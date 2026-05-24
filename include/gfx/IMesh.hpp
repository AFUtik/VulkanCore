#pragma once

#include <cstdint>

namespace gfx 
{

struct IMesh
{
    virtual void updateVertexBuffer   (const void* vertices, uint64_t size)  = 0;
    virtual void updateIndexBuffer    (const void* indices, uint64_t size)   = 0;
    virtual void updateInstanceBuffer (const void* instances, uint64_t size) = 0;

    virtual ~IMesh() = default;
};

}