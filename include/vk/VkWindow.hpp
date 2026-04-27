#pragma once

#include <vulkan/vulkan.h>

namespace myvk {

struct VkWindow {
    static void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
	static VkExtent2D getExtent();
};

}