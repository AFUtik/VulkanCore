#pragma once

#include "management/Handle.hpp"
#include <vector>

namespace gfx 
{

struct Pipeline;

struct ResourceSet
{
    struct Binding
    {
        enum class Type
        {
            Texture,
            UniformBuffer,
            StorageBuffer
        } type;
        Handle<unknown_type> dataHandle;
    };
    std::vector<Binding> bindings;
};

struct MaterialDesc
{
    Handle<Pipeline>    pipeline;
    Handle<ResourceSet> resourceSet;

    virtual void update() = 0;
};

}