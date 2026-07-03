#pragma once

#include "IDevice.hpp"
#include "pch.hpp"

struct Window;

namespace gfx 
{

extern std::unique_ptr<Device> createOpenGLBackend(Window* windowInstance);

}
