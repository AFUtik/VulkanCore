#pragma once

#include "ComponentManager.hpp"
#include <glm/glm.hpp>

struct PlanetComponentManager : ComponentManager<
    PlanetComponentManager, 
    u64, 
    Empty,
    DYNAMIC_SIZE> 
{
    static PlanetComponentManager& instance()
    {
        static PlanetComponentManager manager;
        return manager;
    }
};

using PCM = PlanetComponentManager;

template <typename T>
using PlanetComponent = PCM::Component<T>;

struct PCVelocity : PCM::PlainComponent<PCVelocity>
{
    glm::dvec2 velocity;

    PCVelocity(glm::dvec2 vel) : velocity(vel) {}
};

struct PCAcceleration : PCM::PlainComponent<PCAcceleration>
{
    glm::dvec2 acceleration;
    
    PCAcceleration(glm::dvec2 acc) : acceleration(acc) {}
};

struct PCPosition : PCM::PlainComponent<PCPosition>
{
    glm::dvec2 previous = glm::dvec2(.0);
    glm::dvec2 position;
    
    PCPosition(glm::dvec2 pos) : position(pos) {}

    inline glm::dvec2 interpolate(double t) const
    {
        return glm::mix(previous, position, t);
    }

    inline void updatePosition(const glm::dvec2 &pos)
    {
        previous = position;
        position = pos;
    }
};

struct PCProperties : PCM::PlainComponent<PCProperties>
{
    float radius = 1.0f;
    double mass  = 0.0;

    PCProperties(float radius, double mass) : radius(radius), mass(mass) {}
};

struct PCColor : PCM::PlainComponent<PCColor>
{
    glm::vec3 color;

    PCColor(glm::vec3 color) : color(color) {}
};

static inline void registerPlanetComponents()
{
    auto& pcm = PCM::instance();
    pcm.register_type<PCVelocity>();
    pcm.register_type<PCAcceleration>();
    pcm.register_type<PCPosition>();
    pcm.register_type<PCProperties>();
    pcm.register_type<PCColor>();
}