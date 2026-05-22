#include "game/GameContext.hpp"
#include "game/PCManager.hpp"

#include "ThreadPool.hpp"

GameContext::GameContext() : pcm(PCM::instance())
{
    qt.init(bounds, 8, 8);
}

void GameContext::tick()
{
    auto& objects = pcm.get_objects();
    const uint32_t obj_count = objects.size();

    threadPool.parallel_for(0, obj_count, [&](size_t i) {
        auto& object = objects[i];
        auto& vel = pcm.get_component<PCVelocity>(object).velocity;
        auto& acc = pcm.get_component<PCAcceleration>(object).acceleration;
        vel += acc * (0.5 * tickPhisicsDelta);
    });

    threadPool.parallel_for(0, obj_count, [&](size_t i) {
        auto& object = objects[i];
        auto& pos_c = pcm.get_component<PCPosition>(object);
        auto& vel_c = pcm.get_component<PCVelocity>(object);
        pos_c.setPosition(pos_c.position + vel_c.velocity * tickPhisicsDelta);
    });

    qt.clear();
    qt.nodes_barnes_.resize(1);

    for (auto& object : pcm.get_objects())
    {
        qt.insertBodyInto(
            0, 
            object.get_id(),
            pcm.get_component<PCPosition>(object).position,
            pcm.get_component<PCProperties>(object).mass
        );
    }
        
    threadPool.parallel_for(0, obj_count, [&](size_t i) {
        auto& object = objects[i];
        pcm.get_component<PCAcceleration>(object).acceleration = glm::dvec2(0.0, 0.0);
    });

    threadPool.parallel_for(0, obj_count, [&](size_t i) {
        auto& object = objects[i];
        qt.computeForce(
            0,
            object.get_id(),
            pcm.get_component<PCPosition>(object).position,
            pcm.get_component<PCAcceleration>(object).acceleration,
            pcm.get_component<PCProperties>(object).mass
        );
    });

    threadPool.parallel_for(0, obj_count, [&](size_t i) {
        auto& object = objects[i];
        auto& vel = pcm.get_component<PCVelocity>(object).velocity;
        auto& acc = pcm.get_component<PCAcceleration>(object).acceleration;
        vel += acc * (0.5 * tickPhisicsDelta);
    });
}
