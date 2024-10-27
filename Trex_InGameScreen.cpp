#include "Trex_InGameScreen.h"

Engine::Trex_InGameScreen::Trex_InGameScreen()
{
}
Engine::Trex_InGameScreen::~Trex_InGameScreen() {
	delete dinoTexture;
	delete dinoSprite;
	delete rintanganTexture;

	delete dotTexture;
	delete dotSprite1;
	delete dotSprite2;
	delete dotSprite3;
	delete dotSprite4;
	delete music;
	delete text;
	delete messageTryAgain;
}
void Engine::Trex_InGameScreen::Init()
{
	//Dino Texture
	Texture* dinoTexture = new Texture("Dino.png");
	dinoSprite = new Sprite(dinoTexture, game->GetDefaultSpriteShader(), game->GetDefaultQuad());
	dinoSprite->SetPosition(game->GetSettings()->screenWidth - (0.85 * game->GetSettings()->screenWidth), 0)->SetNumXFrames(6)->SetNumYFrames(1)->AddAnimation("idle", 0, 0)->AddAnimation("walk", 0, 2)->AddAnimation("jump", 3, 3)->AddAnimation("down", 4, 5)->SetScale(1.2f)->SetAnimationDuration(90);

	// Spawn setting
	maxSpawnTime = 1600;
	numObjectPerSpawn = 1;
	numObjectsInPool = 10;
	//Rintangan Texture
	rintanganTexture = new Texture("Cactus3.png");

	// This dot sprite is for visual debugging (to see the actual bounding box) 
	dotTexture = new Texture("dot.png");
	dotSprite1 = new Sprite(dotTexture, game->GetDefaultSpriteShader(), game->GetDefaultQuad());
	dotSprite2 = new Sprite(dotTexture, game->GetDefaultSpriteShader(), game->GetDefaultQuad());
	dotSprite3 = new Sprite(dotTexture, game->GetDefaultSpriteShader(), game->GetDefaultQuad());
	dotSprite4 = new Sprite(dotTexture, game->GetDefaultSpriteShader(), game->GetDefaultQuad());
	// Add input mapping 
	game->GetInputManager()->AddInputMapping("Jump", SDLK_d)->AddInputMapping("Down", SDLK_a )->AddInputMapping("Attack", SDLK_x)->AddInputMapping("Quit", SDLK_ESCAPE)->AddInputMapping("tryAgain", SDLK_SPACE);

	//untuk score
	inputFile.open("score.txt");
	inputFile >> highestScore;
	inputFile.close();
	outputFile.open("score.txt");
	score = 0;
	lastScoreUpdate = 0.0f;
	scoreUpdateInterval = 1.0f;
	percepatan = 0.0f;

	//Set Background
	for (int i = 0; i <= 4; i++) {
		AddToLayer(backgrounds, "bg0" + to_string(i) + ".png");
	}
	AddToLayer(backgrounds, "Bg_1.png");
	for (int i = 5; i <= 8; i++) {
		AddToLayer(middlegrounds, "bg0" + to_string(i) + ".png");
	}
	AddToLayer(foregrounds, "GroundImage.png");

	text = new Text("lucon.ttf", 36, game->GetDefaultTextShader());
	text->SetScale(1.0f)->SetColor(255, 255, 255);

	textPixel = new Text("8-bit Arcade In.ttf", 72, game->GetDefaultTextShader());
	textPixel->SetScale(1.0f)->SetColor(255, 255, 255);

	textHighestScore = new Text("lucon.ttf", 28, game->GetDefaultTextShader());
	textHighestScore->SetScale(1.0f)->SetColor(255, 5, 50);

	messageTryAgain = new Text("lucon.ttf", 36, game->GetDefaultTextShader());
	messageTryAgain->SetScale(1.0f)->SetColor(255, 255, 255);

	music = (new Music("One Must Fall 2097 (1986 remix) - NES_Famitracker.ogg"))->SetVolume(40)->Play(true);

	sound = (new Sound("jump.wav"))->SetVolume(100);

	offset = 2;
	debug = true;

}

void Engine::Trex_InGameScreen::Update()
{
	dinoSprite->SetBoundingBoxSize(dinoSprite->GetScaleWidth() - (16 * dinoSprite->GetScale()),
		dinoSprite->GetScaleHeight() - (4 * dinoSprite->GetScale()));
	
	if (state == Trex_InGameScreen::State::GAME_OVER)
	{
		if (score > highestScore) {
			outputFile << score << endl;
			outputFile.close();
		}
		if (game->GetInputManager()->IsKeyPressed("tryAgain"))
		{
			RestartGame();  // Fungsi untuk mereset permainan
		}
		else if (game->GetInputManager()->IsKeyReleased("Quit"))
		{
			ScreenManager::GetInstance(game)->SetCurrentScreen("mainmenu");
			RestartGame();
			return;
		}
		return;
	}
	else if (state == Trex_InGameScreen::State::PAUSE)
	{
		if (game->GetInputManager()->IsKeyPressed("tryAgain"))
		{
			state = State::PLAY;;  // Melanjutkan permainan
		}
		else if (game->GetInputManager()->IsKeyReleased("Quit"))
		{
			ScreenManager::GetInstance(game)->SetCurrentScreen("mainmenu");
			RestartGame();
			return;
		}
		return;
	}

	if (game->GetInputManager()->IsKeyReleased("Quit"))
	{
		state = State::PAUSE;
		return;
	}

	//Collide antara dino dan rintangan
	for (Turtle* s : objects)
	{
		if (dinoSprite->GetBoundingBox()->CollideWith(s->GetBoundingBox())) {
			state = State::GAME_OVER;
			return;
		}
	}


	// Time to spawn objects
	if (spawnDuration >= maxSpawnTime) {
		SpawnObjects();
		spawnDuration = 0;
	}
	// Update all objects
	for (Turtle* o : objects) {
		o->Update(game->GetGameTime(), percepatan);
	}
	// Count spawn duration
	spawnDuration += game->GetGameTime();

	//SCORE
	float deltaTime = game->GetGameTime() / 1000.0f;
	lastScoreUpdate += deltaTime;

	//Tambah 1 score setiap 1 detik
	if (lastScoreUpdate >= scoreUpdateInterval) {
		score += 1;
		lastScoreUpdate = 0.0f;
		if (maxSpawnTime > 500) {
			maxSpawnTime -= 20;//Percepat Spawn Rintangan
			percepatan += 0.008f;//Percepat Laju rintangan
		}
	}
	if (score > highestScore) {
		highestScore = score;
	}
	text->SetText("Score = " + to_string(score));//Score adding
	textHighestScore->SetText("Highest Score = " + to_string(highestScore));

	dinoSprite->PlayAnim("walk");

	vec2 oldMonsterPos = dinoSprite->GetPosition();
	float x = oldMonsterPos.x, y = oldMonsterPos.y;
	dinoSprite->SetPosition(x, y);

	//Jika tombol jump ditekan pertama kali, inisialisasi lompatan
	if (game->GetInputManager()->IsKeyPressed("Jump") && !jump) {
		float ratio = (game->GetGameTime() / 16.7f);
		gravity = 0.16f * ratio;
		yVelocity = 1.6f;
		jump = true;
		sound->Play(false);
		jumpKeyHeld = true;
		dinoSprite->PlayAnim("jump");
	}
	if (game->GetInputManager()->IsKeyPressed("Jump") && jumpKeyHeld) {
		yVelocity += 0.13f; //tambah kecepatan lompatan
		if (yVelocity > 1.6f) {//batasi kecepatan lompatan
			yVelocity = 1.6f;
		}
	}

	if (game->GetInputManager()->IsKeyReleased("Jump")) {
		jumpKeyHeld = false;
	}

	if (y > 0 && jumpKeyHeld) {
		float ratio = (game->GetGameTime() / 16.7f);
		jumpKeyHeldGravity = 0.12f * ratio;
		yVelocity -= gravity;
	}

	else if (y < 0) {
		jump = false;
		yVelocity = 0;
		y = 0;
	}
	else if (y > 0) {
		yVelocity -= gravity;
	}

	if (game->GetInputManager()->IsKeyPressed("Down")) {
		dinoSprite->PlayAnim("down");
		dinoSprite->SetBoundingBoxSize(dinoSprite->GetScaleWidth() - (16 * dinoSprite->GetScale()),
			dinoSprite->GetScaleHeight() - (32 * dinoSprite->GetScale()));
		if (y > 0) {
			keyDownGravity = 0.21;
			yVelocity -= keyDownGravity*(game->GetGameTime() / 16.7f);
		}
	}

	y += yVelocity * game->GetGameTime();

	dinoSprite->SetPosition(x, y);

	dinoSprite->Update(game->GetGameTime());

	MoveLayer(backgrounds, 0.01f);
	MoveLayer(middlegrounds, 0.03f);
	MoveLayer(foregrounds, 0.5f);

	if (debug) {
		//Shape for debug
		BoundingBox* bb = dinoSprite->GetBoundingBox();
		dotSprite1->SetPosition(bb->GetVertices()[0].x - (0.5f * dotSprite1->GetScaleWidth()),
			bb->GetVertices()[0].y - (0.5f * dotSprite1->GetScaleHeight()));
		dotSprite2->SetPosition(bb->GetVertices()[1].x - (0.5f * dotSprite2->GetScaleWidth()),
			bb->GetVertices()[1].y - (0.5f * dotSprite2->GetScaleHeight()));
		dotSprite3->SetPosition(bb->GetVertices()[2].x - (0.5f * dotSprite3->GetScaleWidth()),
			bb->GetVertices()[2].y - (0.5f * dotSprite3->GetScaleHeight()));
		dotSprite4->SetPosition(bb->GetVertices()[3].x - (0.5f * dotSprite4->GetScaleWidth()),
			bb->GetVertices()[3].y - (0.5f * dotSprite3->GetScaleHeight()));
	}

}

void Engine::Trex_InGameScreen::Draw()
{
	if (state == State::PLAY) {
		for (int i = 0; i < numObjectsInPool; i++) {
			Turtle* o = new Turtle(CreateSprite());
			objects.push_back(o);
		}
		DrawLayer(backgrounds);
		DrawLayer(middlegrounds);
		//DrawLayer(foregrounds);

		// Render skor
		text->SetPosition((game->GetSettings()->screenWidth * 0.5f) - (text->GetTextWidth() * 0.5f),
			game->GetSettings()->screenHeight - (text->GetFontSize() * text->GetScale() + 50));

		textHighestScore->SetPosition((game->GetSettings()->screenWidth * 0.5f) - (textHighestScore->GetTextWidth() * 0.5f), 
			game->GetSettings()->screenHeight - (text->GetFontSize() * text->GetScale() + (textHighestScore->GetFontSize() * textHighestScore->GetScale()) + 50));

		if (debug) {
			dotSprite1->Draw();
			dotSprite2->Draw();
			dotSprite3->Draw();
			dotSprite4->Draw();
		}

		text->Draw();
		textHighestScore->Draw();
		// Render all objects
		for (Turtle* o : objects) {
			o->Draw();
		}

		dinoSprite->Draw();
	}
	else if (state == State::GAME_OVER) {
		//Update score.txt
		if (score > highestScore) {
			outputFile << score << endl;
			outputFile.close();
		}
		textPixel->SetText("Game Over");
		textPixel->SetPosition((game->GetSettings()->screenWidth * 0.5f) - (textPixel->GetTextWidth() * 0.5f), game->GetSettings()->screenHeight * 0.7);
		textPixel->Draw();

		text->SetPosition((game->GetSettings()->screenWidth * 0.5f) - (text->GetTextWidth() * 0.5f), 
			game->GetSettings()->screenHeight * 0.62);
		text->Draw();

		textHighestScore->SetPosition((game->GetSettings()->screenWidth * 0.5f) - (textHighestScore->GetTextWidth() * 0.5f), 
			game->GetSettings()->screenHeight * 0.5 + (textHighestScore->GetFontSize() * textHighestScore->GetScale()));
		textHighestScore->Draw();
		// Gambar pesan Try Again

		messageTryAgain->SetText("Press 'space' to Try Again Or 'esc' to Exit");
		messageTryAgain->SetPosition((game->GetSettings()->screenWidth * 0.5f) - (messageTryAgain->GetTextWidth() * 0.5f), game->GetSettings()->screenHeight * 0.38);
		messageTryAgain->Draw();
		
		return;
	}
	else if (state == State::PAUSE) {
		textPixel->SetText("GAME PAUSE");
		textPixel->SetPosition((game->GetSettings()->screenWidth * 0.5f) - (textPixel->GetTextWidth() * 0.5f), game->GetSettings()->screenHeight * 0.6);
		textPixel->Draw();
		messageTryAgain->SetText("Press Space to RESUME OR Escape to EXIT");
		messageTryAgain->SetPosition((game->GetSettings()->screenWidth * 0.5f) - (messageTryAgain->GetTextWidth() * 0.5f), game->GetSettings()->screenHeight * 0.45);
		messageTryAgain->Draw();
		return;
	}

}

void Engine::Trex_InGameScreen::RestartGame() {
	// Reset skor & Spawntime
	objects.clear();
	score = 0;
	maxSpawnTime = 1600;
	percepatan = 0.0f;
	//untuk score
	inputFile.open("score.txt");
	inputFile >> highestScore;
	inputFile.close();
	outputFile.open("score.txt");
	// Reset posisi Dino dan rintangan
	dinoSprite->SetPosition(game->GetSettings()->screenWidth - (0.85 * game->GetSettings()->screenWidth), 0);

	for (Turtle* s : objects) {
		s->SetPosition(game->GetSettings()->screenWidth + rand() % 100, 0);  // Posisi baru untuk rintangan
	}

	// Kembali ke state PLAY
	state = State::PLAY;
}
//Parallax
void Engine::Trex_InGameScreen::MoveLayer(vector<Sprite*>& bg, float speed)
{
	for (Sprite* s : bg) {
		if (s->GetPosition().x < -game->GetSettings()->screenWidth + offset) {
			s->SetPosition(game->GetSettings()->screenWidth + offset - 1, 0);
		}
		s->SetPosition(s->GetPosition().x - speed * game->GetGameTime(), s->GetPosition().y);
		s->Update(game->GetGameTime());
	}
}
void Engine::Trex_InGameScreen::DrawLayer(vector<Sprite*>& bg)
{
	for (Sprite* s : bg) {
		s->Draw();
	}
}

void Engine::Trex_InGameScreen::AddToLayer(vector<Sprite*>& bg, string name)
{
	Texture* texture = new Texture(name);

	Sprite* s = new Sprite(texture, game->GetDefaultSpriteShader(), game->GetDefaultQuad());
	s->SetSize(game->GetSettings()->screenWidth + offset, game->GetSettings()->screenHeight);
	bg.push_back(s);

	Sprite* s2 = new Sprite(texture, game->GetDefaultSpriteShader(), game->GetDefaultQuad());
	s2->SetSize(game->GetSettings()->screenWidth + offset, game->GetSettings()->screenHeight)->SetPosition(game->GetSettings()->screenWidth + offset - 1, 0);
	bg.push_back(s2);
}

//ObjectPool
Engine::Sprite* Engine::Trex_InGameScreen::CreateSprite()
{
	// Membuat sprite rintangan
	rintanganSprite = new Sprite(rintanganTexture, game->GetDefaultSpriteShader(), game->GetDefaultQuad());

	rintanganSprite->SetNumXFrames(1)
		->SetNumYFrames(1)
		->AddAnimation("idle", 0, 0)
		->PlayAnim("idle")
		->SetScale(1.5f)
		->SetAnimationDuration(100);

	// Mengatur bounding box berdasarkan ukuran sprite
	rintanganSprite->SetBoundingBoxSize(
		rintanganSprite->GetScaleWidth() - (16 * rintanganSprite->GetScale()),
		rintanganSprite->GetScaleHeight() - (4 * rintanganSprite->GetScale())
	);

	return rintanganSprite;
}


void Engine::Trex_InGameScreen::SpawnObjects()
{
	//Find Die object to reuse for spawning
	int spawnCount = 0;
	for (Turtle* o : objects) {
		if (spawnCount == numObjectPerSpawn) {
			break;
		}
		if (o->IsDie()) {
			// Set state to spawn
			o->SetSpawn();
			float x = game->GetSettings()->screenWidth * 1.1f;
			float y = 0;
			o->SetPosition(x, y);
			spawnCount++;
		}
	}
}
