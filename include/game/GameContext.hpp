#pragma once

#include "collections/spatial/QuadTree.hpp"
#include <glm/glm.hpp>

struct PlanetComponentManager;

struct GameContext 
{
    using PCM  = PlanetComponentManager;
    using QT   = QuadTree<u32, glm::dvec2, double, glm::dvec2>;
    using AABB = AABB2D<double, glm::dvec2>;

    PCM& pcManager;

    AABB bounds = {-1000.0f, -1000.0f, 1000.0f, 1000.0f};
    
    QT qt;

    const double tickPhysicsPerSecond = 20.0;
    const double tickPhisicsDelta = 1.0 / tickPhysicsPerSecond;
    const bool runPhysicsThreaded = false;

    GameContext();

    void buildQTree();
    void tick();
};