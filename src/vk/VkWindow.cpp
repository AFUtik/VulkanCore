#include "VkWindow.hpp"

#include <stdexcept>

namespace myvk {

void VkWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) 
{
	if (glfwCreateWindowSurface(instance, Window::getGlfwWindow(), nullptr, surface) != VK_SUCCESS) {
		throw std::runtime_error("failed to craete window surface");
	}
}

VkExtent2D VkWindow::getExtent() 
{ 
    return { static_cast<uint32_t>(Window::getWidth()), static_cast<uint32_t>(Window::getHeight()) }; 
}

}