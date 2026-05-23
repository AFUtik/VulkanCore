#pragma once

#include "BarnesHut.hpp"
#include <glm/glm.hpp>

struct PlanetComponentManager;

struct GameContext 
{
    using PCM  = PlanetComponentManager;
    using QT   = BarnesHutQT;
    using AABB = QT::AABB;

    AABB bounds = {-100000.0f, -100000.0f, 100000.0f, 100000.0f};

    PCM& pcm;
    BarnesHutQT qt;

    static constexpr double tickPhysicsPerSecond = 2.0;
    static constexpr double tickPhisicsDelta = 1.0 / tickPhysicsPerSecond;
    double alpha = 0.0;

    GameContext();
    
    void tick();
};