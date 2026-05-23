#include "gfx/gfx.hpp"
#include "gfx/VulkanBackend.hpp"

namespace gfx 
{

Context gfx;

void initVulkanBackend()
{
    gfx.iRenderDevice = std::make_unique<gfx_vk::VulkanRenderDevice>();
}

}