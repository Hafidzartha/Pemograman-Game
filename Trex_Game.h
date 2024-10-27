#ifndef Trex_Game_H
#define Trex_Game_H

#include "Game.h"
#include "Setting.h"
#include "Trex_InGameScreen.h"
#include "Trex_MainMenuScreen.h"

#include <vector>

namespace Engine {
	class Trex_Game :public Engine::Game
	{
	public:
		Trex_Game(Setting* setting);
		~Trex_Game();
		virtual void Init();
		virtual void Update();
		virtual void Render();
		
	};
}

#endif // Trex_Game_H
