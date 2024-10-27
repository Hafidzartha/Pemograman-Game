#include "Gui.h"

Engine::Gui::Gui(Setting* setting) : Engine::Game(setting) {
	setting->windowTitle = "Trex Run";
}
Engine::Gui::~Gui() {}

void Engine::Gui::Init() {
	Engine::ScreenManager::GetInstance(this)->AddScreen("ingame", new Trex_Run())
		->AddScreen("mainmenu", new MainMenu())->SetCurrentScreen("mainmenu");
}

void Engine::Gui::Update() {
	Engine::ScreenManager::GetInstance(this)->Update();
}

void Engine::Gui::Render() {
	Engine::ScreenManager::GetInstance(this)->Draw();
}
