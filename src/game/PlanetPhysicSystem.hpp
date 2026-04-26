#pragma once

#include <thread>

struct PlanetPhysicSystem 
{
    PlanetPhysicSystem();
    ~PlanetPhysicSystem();

    void runThread(double delta);

    void update();

    std::thread physicThread;
    bool running = false;
}; 