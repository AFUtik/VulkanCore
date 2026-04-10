#pragma once

#include <vulkan/vulkan.h>

namespace myvk {

struct FrameInfo {
  int frameIndex = 0;
  float frameTime = 0.0f;
  VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
};

}
