#include "Game.hpp"
#include <RVL/Core/EntryPoint.hpp>

using namespace Rvl;

RVL_IMPL_INIT(Game);

Game::Game() : App(1000, 700, "rvl game") {}
Game::~Game() {}

void Game::Start()
{
    SetClearColor({0.1f, 0.1f, 0.1f});
    _currentState = NewPtr<MainState>();
    _currentState->Start();
}

void Game::Update()
{
    _currentState->Update();
}

void Game::Tick()
{
    _currentState->Tick();
}