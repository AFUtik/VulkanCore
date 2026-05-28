#include "gfx/gfx.hpp"
#include "gfx/backend/VulkanBackend.hpp"

#include "gfx/vk/Renderer.hpp"
#include "gfx/vk/Device.hpp"

namespace gfx 
{

Context gfx;

void initVulkanBackend()
{
    auto device = std::make_unique<gfx_vk::VulkanRenderDevice>();
    device->renderer = std::make_unique<vk::Renderer>();
    gfx.iRenderDevice = std::move(device);
    gfx.backendType = BackendType::Vulkan;
}

void freeVulkanBackend()
{
    vkDeviceWaitIdle(vk::Device::instance().device());
    gfx.iRenderDevice.reset();
}

}