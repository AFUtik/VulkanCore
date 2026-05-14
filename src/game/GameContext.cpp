#include "game/GameContext.hpp"
#include "game/PCManager.hpp"
#include "game/PCSystem.hpp"

GameContext::GameContext() : pcManager(PCM::instance())
{
    qt.init(bounds, 8, 8);
}

void GameContext::tick()
{
    qt.step(tickPhisicsDelta);
}
