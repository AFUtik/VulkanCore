#pragma once

#include <memory>

namespace gfx 
{

struct IRenderDevice;

enum BackendType
{
    OpenGL, // not implemented
    Vulkan
};

struct Context
{
    std::unique_ptr<IRenderDevice> iRenderDevice;
    BackendType backendType;
};

extern Context gfx;

extern void initVulkanBackend();
extern void freeVulkanBackend();

extern void initOpenglBackend();
extern void freeOpenglBackend();

}

