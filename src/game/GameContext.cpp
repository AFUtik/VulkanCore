#include "game/GameContext.hpp"
#include "game/PCManager.hpp"
#include "game/PCSystem.hpp"

GameContext::GameContext() : pcManager(PCM::instance())
{

}

void GameContext::buildQTree()
{
    qt.init(bounds, 4, 4);
    for(const auto& planet : pcManager.get_objects())
    {
        qt.insert(planet.get_id(), pcManager.template get_component<PCPosition>(planet).position);
    }
}