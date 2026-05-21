#pragma once

#include "vk/FrameInfo.hpp"
#include <glm/glm.hpp>

struct RenderState
{
    myvk::FrameInfo& frame;
    glm::mat4 projview;

    uint32_t rsystem;
    uint32_t pipeline;
};