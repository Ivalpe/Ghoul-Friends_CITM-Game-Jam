#include "UIManager.h"
#include "Engine.h"
#include "Textures.h"
#include "Scene.h"

UIManager::UIManager(): Module() {
	name = "uiManager";
}

UIManager::~UIManager() {}

bool UIManager::Start() {
	lifebar = Engine::GetInstance().textures.get()->Load("Assets/Textures/lifebar.png");

	return true;
}

bool UIManager::Update(float dt) {
	float lifePlayer = Engine::GetInstance().scene->GetPlayerLife();
	float maxLifePlayer = Engine::GetInstance().scene->GetPlayerMaxLife();
	float barWidth = 50;
	SDL_Rect barRect = { 0,0,68,8 };

	if (Engine::GetInstance().scene.get()->GetGameState() == GameState::START) {
		//lifebar render
		Engine::GetInstance().render.get()->DrawTexture(lifebar, 10, 10, SDL_FLIP_NONE, &barRect, false);

		//Activate flashing animation when player is damaged
		if (Engine::GetInstance().scene.get()->IsPlayerDamaged()) lifebarFlash = true;

		//calculate life bar (full = 50)
		SDL_Rect bar = { 11, 10, (int)((lifePlayer / maxLifePlayer) * barWidth), 6 };

		if (lifebarFlash) {
			++flashDuration;
			if (flashDuration >= 12) {
				flashDuration = 0;
				if (iconFlash) {
					iconFlash = false;
					++flashesCount;
				}
				else iconFlash = true;
			}

			if (iconFlash) bar = { 11, 18, (int)((lifePlayer / maxLifePlayer) * barWidth), 6};

			if (flashesCount >= 4) {
				lifebarFlash = false;
			}
		}
		
		Engine::GetInstance().render.get()->DrawTexture(lifebar, 54, 14, SDL_FLIP_NONE, &bar, false);
		std::string lifePoints = std::to_string(Engine::GetInstance().scene.get()->GetPlayerLife());
		Engine::GetInstance().render.get()->DrawText(lifePoints.c_str(), 80, 15, 100, 35);
	}

	return true;
}

bool UIManager::CleanUp() {
	return true;
}
