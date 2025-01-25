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
		//calculate life bar (full = 39)
		SDL_Rect bar = { 11, 10, 0.39 * Engine::GetInstance().scene.get()->GetPlayerLife(), 6 };
		Engine::GetInstance().render.get()->DrawTexture(lifebar, 54, 14, SDL_FLIP_NONE, &bar, false);
	}

	return true;
}

bool UIManager::CleanUp() {
	return true;
}
