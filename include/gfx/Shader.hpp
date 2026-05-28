#pragma once

#include <cstdint>
#include <vector>
#include <string>

namespace gfx 
{

enum class ShaderStage
{
    Vertex,
    Fragment,
    Geometry,
    TessControl,
    TessEvaluation,
    Compute,
    RTX
};

struct Shader
{
    ShaderStage stage;
    std::string path;

    virtual void Compile() = 0;
};

struct ResourceSlot
{
    enum class Type
    {
        Texture,
        UniformBuffer,
        StorageBuffer
    };

    uint32_t set;
    uint32_t binding;

    Type type;
    ShaderStage stages;
};

struct ShaderResourceSignature
{
    std::vector<ResourceSlot> sets;
};

}