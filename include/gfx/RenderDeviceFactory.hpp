#pragma once

#include "IRenderDevice.hpp"

namespace gfx {

enum class Backend 
{ 
    Vulkan, 
    OpenGL
};

std::unique_ptr<IRenderDevice> createRenderDevice(Backend backend);

} // namespace gfx