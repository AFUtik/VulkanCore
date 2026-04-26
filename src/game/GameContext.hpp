#pragma once

#include "PlanetPhysicSystem.hpp"
#include "Planet.hpp"

struct GameContext 
{
    //PlanetPhysicSystem planetPhysics;

    const double tickPhysicsPerSecond = 20.0;
    const double tickPhisicsDelta = 1.0 / tickPhysicsPerSecond;
    const bool runPhysicsThreaded = false;
};