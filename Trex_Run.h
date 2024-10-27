#ifndef TREX_RUN_H
#define TREX_RUN_H

#include "Setting.h"
#include "Texture.h"
#include "Sprite.h"
#include "Turtle.h"
#include "Screen.h"
#include "ScreenManager.h"
#include "Text.h"
#include "Music.h"
#include "Sound.h"
#include <fstream>

namespace Engine
{
	class Trex_Run :
		public Engine::Screen
	{
	public:
		Trex_Run();
		~Trex_Run();
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

		Texture* dinoTexture = NULL;
		Sprite* dinoSprite = NULL;
		Sprite* bgSprite = NULL;
		Sprite* CreateSprite();
		Texture * rintanganTexture = NULL;
		Sprite * rintanganSprite = NULL;
		vector<Turtle*> objects;
		vector<Sprite*> rintangan;

		float spawnDuration = 0, maxSpawnTime = 0, numObjectsInPool = 0, numObjectPerSpawn = 0, offset = 0;
		
		Text* text = NULL;
		Text* textPixel = NULL;
		Text* textHighestScore = NULL;
		Text* messageTryAgain = NULL;
		ifstream inputFile;
		ofstream outputFile;
		Music* music = NULL;;
		Sound* sound = NULL;
		int score = 0, highestScore = 0;
		float mul = 1, percepatan = 0;

		vector<Sprite*> backgrounds, middlegrounds, foregrounds;

		float yVelocity = 0, gravity = 0, jumpKeyHeldGravity = 0 ,keyDownGravity = 0, lastScoreUpdate = 0, scoreUpdateInterval = 0;
		bool jump = false, jumpKeyHeld = false, debug = false;

		void SpawnObjects();

		void MoveLayer(vector<Sprite*>& bg, float speed);
		void DrawLayer(vector<Sprite*>& bg);
		void AddToLayer(vector<Sprite*>& bg, string name);
		void RestartGame();
	};
}
#endif

