#ifndef TREX_INGAMESCREEN_H
#define TREX_INGAMESCREEN_H

#include "Screen.h"
#include "Turtle.h"
#include "Sprite.h"
#include "ScreenManager.h"
#include "Setting.h"
#include "Texture.h"
#include "Text.h"
#include "Music.h"
#include "Sound.h"
#include <fstream>

namespace Engine {
	class Trex_InGameScreen :
		public Engine::Screen
	{
	public:
		Trex_InGameScreen();
		~Trex_InGameScreen();
		void Init();
		void Update();
		void Draw();
	private:
		enum class State {
			PLAY,
			GAME_OVER,
			PAUSE
		};

		State state = State::PLAY;

		Engine::Texture* dinoTexture = NULL;
		Engine::Sprite* dinoSprite = NULL;
		Engine::Texture* rintanganTexture = NULL;
		Engine::Sprite* rintanganSprite = NULL;
		Engine::Sprite* CreateSprite();
		Texture* dotTexture = NULL;
		Sprite* dotSprite1 = NULL;
		Sprite* dotSprite2 = NULL;
		Sprite* dotSprite3 = NULL;
		Sprite* dotSprite4 = NULL;
		Text* text = NULL;
		Text* textPixel = NULL;
		Text* textHighestScore = NULL;
		Text* messageTryAgain = NULL;
		ifstream inputFile;
		ofstream outputFile;
		Music* music = NULL;;
		Sound* sound = NULL;
		vector<Sprite*> backgrounds, middlegrounds, foregrounds;
		vector<Engine::Turtle*> objects;
		vector<Sprite*> rintangan;
		float spawnDuration = 0, maxSpawnTime = 0, numObjectsInPool = 0, numObjectPerSpawn = 0;
		void SpawnObjects();
		void MoveLayer(vector<Sprite*>& bg, float speed);
		void DrawLayer(vector<Sprite*>& bg);
		void AddToLayer(vector<Sprite*>& bg, string name);
		void RestartGame();

		int score = 0, highestScore = 0;

		float mul = 1,percepatan = 0 ;
		float yVelocity = 0, gravity = 0, jumpKeyHeldGravity = 0, keyDownGravity = 0, offset = 0, lastScoreUpdate = 0, scoreUpdateInterval = 0;
		bool jump = false, debug = false, jumpKeyHeld = false;
	};
}

#endif