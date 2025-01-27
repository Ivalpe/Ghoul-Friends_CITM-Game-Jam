#pragma once

#include "Module.h"
#include "Player.h"
#include "Merchant.h"
#include <vector>
#include "Power.h"
#include "Enemy.h"
#include "Chest.h"

struct SDL_Texture;

enum class GameState {
	MAINMENU, START, DEATH
};

class Scene : public Module
{
public:

	Scene();

	// Destructor
	virtual ~Scene();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	void CreateAttack();

	void AddItem(int item);

	void DrawText(bool draw) {
		drawChestText = draw;
	}

	// Called before quitting
	bool CleanUp();

	GameState GetGameState() {
		return state;
	}

	int GetPlayerLife() {
		return player->life;
	}

	int GetPlayerMaxLife() {
		return player->maxLife;
	}

	bool IsPlayerDamaged() {
		return player->isDamaged;
	}

	int PlayerDamagedReceived() {
		return player->damageReceived;
	}

	Vector2D GetPlayerPosition() {
		return player->GetPosition();
	}

private:
	SDL_Texture* drums;
	SDL_Texture* book;
	SDL_Texture* armor;
	SDL_Texture* life;
	SDL_Texture* regeneration;

	//death screen timer
	int deathTime = 0;
	int deathMaxTime = 120;

	//L03: TODO 3b: Declare a Player attribute
	Player* player;
	GameState state;
	std::vector<Power*> fireballList;
	std::vector<Enemy*> enemyList;
	std::vector<Chest*> chestList;
	std::vector<Merchant*> eventsList;
	bool drawChestText = false;
	pugi::xml_document textsParameters;
	std::map<SDL_Texture*, int> itemsList;
};