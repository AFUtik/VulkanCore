#pragma once

#include "Scalar.hpp"
#include "management/ComponentManager.hpp"

struct Planet
{

};

struct PlanetManager : ComponentManager<PlanetManager, uint32_t, Planet, 32, 1024> {};

template<typename T>
using PlanetComponent = PlanetManager::Component<T>;

struct PCVelocity : PlanetComponent<PCVelocity>
{
    glm::dvec2 velocity;
};

struct PCPosition : PlanetComponent<PCPosition>
{
    glm::dvec2 position;
};

struct PCMass : PlanetComponent<PCMass>
{
    double mass;
};

struct PCColor : PlanetComponent<PCColor>
{
    glm::vec3 color;
};

