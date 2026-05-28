#pragma once

#include "gfx/IRenderDevice.hpp"
#include "gfx/management/ResourceManager.hpp"

#include "gfx/vk/RenderTarget.hpp"

namespace vk
{
    struct Mesh;
    struct Texture; 
    struct Renderer;
}

namespace gfx_vk 
{

struct VulkanRenderDevice : public gfx::IRenderDevice
{
    std::unique_ptr<vk::Renderer> renderer;

    gfx::ResourceManager<vk::Mesh, 512>        meshResource;
};

}