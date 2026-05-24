#pragma once

#include "IRenderDevice.hpp"
#include "ResourceManager.hpp"

#include "gfx/IMesh.hpp"

namespace vk
{
    struct Mesh;
}

namespace gfx_vk 
{

struct VulkanRenderDevice : public gfx::IRenderDevice
{
    gfx::ResourceManager<vk::Mesh, 512> meshResource;
};

}