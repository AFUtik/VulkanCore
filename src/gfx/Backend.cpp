#include "gfx/Backend.hpp"
#include "gfx/backend/opengl/BackendGL.hpp"

namespace gfx
{

std::unique_ptr<Device> createOpenGLBackend(Window* windowInstance)
{
    return std::make_unique<gl::BackendGL>(windowInstance);
}

}
