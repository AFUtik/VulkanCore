#pragma once

#include "rendering/Renderer.hpp"

struct Global
{
    std::unique_ptr<Renderer> renderer;

    void shutdown();

    Global();
};

static Global global;