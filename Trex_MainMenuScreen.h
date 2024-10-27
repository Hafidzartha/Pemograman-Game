#ifndef TREX_MAINMENUSCREEN_H
#define TREX_MAINMENUSCREEN_H

#include "Screen.h"
#include "Game.h"
#include "Setting.h"
#include "Texture.h"
#include "Sprite.h"
#include "Button.h"
#include "Text.h"
#include <vector>
#include "ScreenManager.h"

namespace Engine {
	class Trex_MainMenuScreen :
		public Engine::Screen
	{
	public:
		Trex_MainMenuScreen();
		void Init();
		void Update();
		void Draw();
	private:
		Engine::Texture* background = NULL;
		Engine::Sprite* backgroundSprite = NULL;
		vector<Button*> buttons;
		Text* text;
		int currentButtonIndex = 0;
	};
}

#endif