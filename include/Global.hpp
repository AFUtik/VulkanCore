#pragma once

#include "game/GameContext.hpp"
#include "rendering/Renderer.hpp"
#include "Assets.hpp"

struct Global
{
    GameContext gameCtx;
    Assets assets;

    std::unique_ptr<Renderer> renderer;

    void loadAssets();

    void shutdown();

    Global();
};

extern Global global;