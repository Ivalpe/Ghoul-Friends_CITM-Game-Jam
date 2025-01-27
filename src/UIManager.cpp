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
	if (Engine::GetInstance().scene.get()->GetGameState() == GameState::START) {
		//lifebar render
		Engine::GetInstance().render.get()->DrawTexture(lifebar, 10, 10, SDL_FLIP_NONE, &barRect, false);

		//Activate flashing animation when player is damaged
		if (Engine::GetInstance().scene.get()->IsPlayerDamaged()) lifebarFlash = true;

		//calculate life bar (full = 39)
		SDL_Rect bar = { 11, 10, 0.39 * Engine::GetInstance().scene.get()->GetPlayerLife(), 6 };

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

			if (iconFlash) bar = { 11, 18, (int)(0.39 * (Engine::GetInstance().scene.get()->GetPlayerLife() + Engine::GetInstance().scene.get()->PlayerDamagedReceived())), 6 };

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
