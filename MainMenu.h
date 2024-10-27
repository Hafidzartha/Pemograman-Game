#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "Game.h"
#include "Setting.h"
#include "texture.h"
#include "sprite.h"
#include "screen.h"
#include "Button.h"
#include "Text.h"
#include <vector>
#include "ScreenManager.h"

namespace Engine {
	class MainMenu :
		public Engine::Screen
	{
	public:
		MainMenu();
		void Init();
		void Update();
		void Draw();

	private:
		Texture* backgound = NULL;
		Sprite* bgSprite = NULL;
		vector<Button* > buttons;
		Text* text;
		int currentButtonIndex = 0;
	};
}

#endif
