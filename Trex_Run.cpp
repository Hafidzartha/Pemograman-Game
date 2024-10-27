#include "Trex_Run.h"

using namespace Engine;

Trex_Run::Trex_Run(){}
Trex_Run::~Trex_Run() {
	delete dinoTexture;
	delete dinoSprite;
	delete rintanganTexture;
	delete rintanganSprite;

	delete music;
	delete text;
	delete messageTryAgain;
}

void Engine::Trex_Run::Init() {
	Texture* dinoTexture = new Texture("Dino.png");
	dinoSprite = new Sprite(dinoTexture, game->GetDefaultSpriteShader(), game->GetDefaultQuad());
	dinoSprite->SetPosition(game->GetSettings()->screenWidth - (0.85 * game->GetSettings()->screenWidth), 0)->SetNumXFrames(6)->SetNumYFrames(1)->AddAnimation("idle", 0, 0)->AddAnimation("walk", 0, 2)->AddAnimation("jump", 3, 3)->AddAnimation("down", 4, 5)->SetScale(1.2f)->SetAnimationDuration(90);

	dinoSprite->SetBoundingBoxSize(dinoSprite->GetScaleWidth() - (16 * dinoSprite->GetScale()), dinoSprite->GetScaleHeight() - (4 * dinoSprite->GetScale()));

	rintanganTexture = new Texture("Cactus3.png");

	maxSpawnTime = 1600;
	numObjectPerSpawn = 1;
	numObjectsInPool = 10;

	//untuk score
	inputFile.open("score.txt");
	inputFile >> highestScore;
	inputFile.close();
	outputFile.open("score.txt");
	score = 0;
	lastScoreUpdate = 0.0f;
	scoreUpdateInterval = 1.0f;
	percepatan = 0.0f;

	for (int i = 0; i <= 4; i++) {
		AddToLayer(backgrounds, "bg0" + to_string(i) + ".png");
	}
	for (int i = 5; i <= 8; i++) {
		AddToLayer(middlegrounds, "bg0" + to_string(i) + ".png");
	}
	for (int i = 9; i <= 9; i++) {
		AddToLayer(foregrounds, "bg0" + to_string(i) + ".png");
	}

	game->GetInputManager()->AddInputMapping("Jump", SDLK_c)->AddInputMapping("Down", SDLK_z)->AddInputMapping("Quit", SDLK_ESCAPE)
		->AddInputMapping("Quit", SDLK_ESCAPE)->AddInputMapping("tryAgain", SDLK_SPACE);

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

void Engine::Trex_Run::Update() {
	if (state == Trex_Run::State::GAME_OVER)
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
	else if (state == Trex_Run::State::PAUSE)
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
	if (game->GetInputManager()->IsKeyReleased("Quit")) {
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
	for (Turtle * o : objects) {
		o->Update(game->GetGameTime(), 0 );
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

	vec2 oldDinoPos = dinoSprite->GetPosition();
	float x = oldDinoPos.x, y = oldDinoPos.y;

	if (game->GetInputManager()->IsKeyPressed("Jump") && !jump) {
		float ratio = (game->GetGameTime() / 16.7f);
		gravity = 0.16f * ratio;
		yVelocity = 1.8f;
		jump = true;
		jumpKeyHeld = true;
		dinoSprite->PlayAnim("jump");
	}

	if (game->GetInputManager()->IsKeyPressed("Jump") && jumpKeyHeld) {
		yVelocity += 0.12f;
		if (yVelocity > 1.6f) {
			yVelocity = 1.6f;
		}
	}
	if (game->GetInputManager()->IsKeyReleased("jump")) {
		jumpKeyHeld = false;
	}
	if (game->GetInputManager()->IsKeyPressed("Down")) {
		dinoSprite->PlayAnim("down");
		dinoSprite->SetBoundingBoxSize(dinoSprite->GetScaleWidth() - (16 * dinoSprite->GetScale()),
			dinoSprite->GetScaleHeight() - (32 * dinoSprite->GetScale()));
		if (y > 0) {
			keyDownGravity = 0.21;
			yVelocity -= keyDownGravity * (game->GetGameTime() / 16.7f);
		}
	}
	if (y > 0 && jumpKeyHeld) {
		float ratio = (game->GetGameTime() / 16.7f);
		jumpKeyHeldGravity = 0.13f * ratio;
		yVelocity -= jumpKeyHeldGravity;
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
			yVelocity -= keyDownGravity * (game->GetGameTime() / 16.7f);
		}
	}
	if (game->GetInputManager()->IsKeyReleased("Down")) {
		dinoSprite->SetBoundingBoxSize(dinoSprite->GetScaleWidth() - (16 * dinoSprite->GetScale()), dinoSprite->GetScaleHeight() - (4 * dinoSprite->GetScale()));
	}

	y += yVelocity * game->GetGameTime();

	dinoSprite->SetPosition(x, y);
	dinoSprite->Update(game->GetGameTime());

	MoveLayer(backgrounds, 0.01f);
	MoveLayer(middlegrounds, 0.03f);
	MoveLayer(foregrounds, 0.5f);
}

void Engine::Trex_Run::Draw() {

	if (state == State::PLAY) {
		for (int i = 0; i < numObjectsInPool; i++) {
			Turtle* o = new Turtle(CreateSprite());
			objects.push_back(o);
		}
		DrawLayer(backgrounds);
		DrawLayer(middlegrounds);
		DrawLayer(foregrounds);

		text->SetPosition((game->GetSettings()->screenWidth * 0.5f) - (text->GetTextWidth() * 0.5f),
			game->GetSettings()->screenHeight - (text->GetFontSize() * text->GetScale() + 50));

		textHighestScore->SetPosition((game->GetSettings()->screenWidth * 0.5f) - (textHighestScore->GetTextWidth() * 0.5f),
			game->GetSettings()->screenHeight - (text->GetFontSize() * text->GetScale() + (textHighestScore->GetFontSize() * textHighestScore->GetScale()) + 50));

		text->Draw();
		textHighestScore->Draw();


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

		text->SetPosition((game->GetSettings()->screenWidth * 0.5f) - (text->GetTextWidth() * 0.5f), game->GetSettings()->screenHeight * 0.62);
		text->Draw();

		textHighestScore->SetPosition((game->GetSettings()->screenWidth * 0.5f) - (textHighestScore->GetTextWidth() * 0.5f), game->GetSettings()->screenHeight * 0.5 + (textHighestScore->GetFontSize() * textHighestScore->GetScale()));
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

void Engine::Trex_Run::RestartGame() {
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

//Parallax Scrolling
void Engine::Trex_Run::MoveLayer(vector<Sprite*>& bg, float speed)
{
	for (Sprite* s : bg) {
		if (s->GetPosition().x < -game->GetSettings()->screenWidth + offset) {
			s->SetPosition(game->GetSettings()->screenWidth + offset - 1, 0);
		}
		s->SetPosition(s->GetPosition().x - speed * game->GetGameTime(), s->GetPosition().y);
		s->Update(game->GetGameTime());
	}
}
void Engine::Trex_Run::DrawLayer(vector<Sprite*>& bg)
{
	for (Sprite* s : bg) {
		s->Draw();
	}
}
void Engine::Trex_Run::AddToLayer(vector<Sprite*>& bg, string name)
{
	Texture* texture = new Texture(name);

	Sprite* s = new Sprite(texture, game->GetDefaultSpriteShader(), game->GetDefaultQuad());
	s->SetSize(game->GetSettings()->screenWidth + offset, game->GetSettings()->screenHeight);
	bg.push_back(s);

	Sprite* s2 = new Sprite(texture, game->GetDefaultSpriteShader(), game->GetDefaultQuad());
	s2->SetSize(game->GetSettings()->screenWidth + offset, game->GetSettings()->screenHeight)->SetPosition(game->GetSettings()->screenWidth + offset - 1, 0);
	bg.push_back(s2);
}

Engine::Sprite* Engine::Trex_Run::CreateSprite()
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

void Engine::Trex_Run::SpawnObjects()
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