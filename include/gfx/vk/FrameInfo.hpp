#pragma once

struct VkCommandBuffer_T;
using VkCommandBuffer = VkCommandBuffer_T*;

namespace vk {

struct FrameInfo {
  int frameIndex = 0;
  float frameTime = 0.0f;
  VkCommandBuffer commandBuffer = nullptr;
};

}
