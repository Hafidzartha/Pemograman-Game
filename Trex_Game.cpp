#include "Trex_Game.h"

Engine::Trex_Game::Trex_Game(Setting* setting) :Engine::Game(setting) {
	setting->windowTitle = "Trex Game";
}
Engine::Trex_Game::~Trex_Game()
{

}

void Engine::Trex_Game::Init()
{
	Engine::ScreenManager::GetInstance(this)->AddScreen("ingame", new Trex_InGameScreen())
		->AddScreen("mainmenu", new Trex_MainMenuScreen())->SetCurrentScreen("mainmenu");

}

void Engine::Trex_Game::Update()
{
	Engine::ScreenManager::GetInstance(this)->Update();

}

void Engine::Trex_Game::Render()
{
	Engine::ScreenManager::GetInstance(this)->Draw();
}

