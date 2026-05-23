#pragma once

#include <cstdint>

namespace gfx
{

struct Mesh
{
    inline void setVertices(const void* vertices, uint64_t count)
    {
        this->vertices = vertices;
        vertexCount = count;
    }

    inline void setIndices(const void* indices, uint64_t count)
    {
        this->indices = indices;
        indexCount = count;
    }

    inline void setInstances(const void* instances, uint64_t count)
    {
        this->instances = instances;
        instanceCount = count;
    }

    inline void setVertexStride(uint64_t stride)
    {
        vertexStride = static_cast<uint8_t>(stride);
    }

    inline void setIndexStride(uint64_t stride)
    {
        vertexStride = static_cast<uint8_t>(stride);
    }

    inline void setInstanceStride(uint64_t stride)
    {
        instanceStride = static_cast<uint8_t>(stride);
    }
protected:
    const void* vertices    = nullptr;
    const void* indices     = nullptr;
    const void* instances   = nullptr;

    uint32_t vertexCount   = 0;
    uint32_t indexCount    = 0;
    uint32_t instanceCount = 0;

    uint8_t vertexStride   = 0;
    uint8_t indexStride    = 0;
    uint8_t instanceStride = 0;
};

}