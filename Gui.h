#ifndef GUI_H
#define GUI_H

#include "Game.h"
#include "Setting.h"
#include "MainMenu.h"
#include "Trex_Run.h"
#include <vector>

namespace Engine {
	class Gui :public Engine::Game
	{
	public:
		Gui(Setting* setting);
		~Gui();
		virtual void Init();
		virtual void Update();
		virtual void Render();
	};
}

#endif
