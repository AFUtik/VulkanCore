#pragma once

#include "IRenderDevice.hpp"
#include "ResourceManager.hpp"

namespace gfx_vk 
{

struct VulkanRenderDevice : public gfx::IRenderDevice
{
gfx::ResourceManager meshResource;
};

}