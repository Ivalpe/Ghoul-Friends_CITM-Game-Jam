#pragma once

#include "Module.h"
#include "Player.h"
#include <vector>
#include "Power.h"
#include "Enemy.h"

struct SDL_Texture;

enum class GameState {
	MAINMENU, START
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

	// Called before quitting
	bool CleanUp();

	GameState GetGameState() {
		return state;
	}

	int GetPlayerLife() {
		return player->life;
	}

private:
	SDL_Texture* img;

	//L03: TODO 3b: Declare a Player attribute
	Player* player;
	GameState state;
	std::vector<Power*> fireballList;
	std::vector<Enemy*> enemyList;
};