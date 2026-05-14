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

    PCM& pcManager;
    BarnesHutQT qt;

    const double tickPhysicsPerSecond = 20.0;
    const double tickPhisicsDelta = 1.0 / tickPhysicsPerSecond;
    const bool runPhysicsThreaded = false;

    GameContext();
    
    void tick();
};