#pragma once

#include <memory>

namespace gfx 
{

struct IRenderDevice;

struct Context
{
    std::unique_ptr<IRenderDevice> iRenderDevice;
};

extern Context gfx;

extern void initVulkanBackend();

}

