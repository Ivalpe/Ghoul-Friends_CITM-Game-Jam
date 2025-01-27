#pragma once

#include "Module.h"
#include "Player.h"
#include "Merchant.h"
#include <vector>
#include "Power.h"
#include "Enemy.h"
#include "Chest.h"
#include "GuiControl.h"
#include "Item.h"

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

	void DrawText(bool draw, pugi::char_t* dialogueSearch = const_cast<pugi::char_t*>("")) {
		drawChestText = draw;
		searchText = dialogueSearch;
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

	bool OnGuiMouseClickEvent(GuiControl* control);

private:
	SDL_Texture* drums;
	SDL_Texture* book;
	SDL_Texture* armor;
	SDL_Texture* life;
	SDL_Texture* regeneration;
	SDL_Texture* buttonTexture;
	SDL_Texture* buttonPressed;
	SDL_Texture* buttonShop;

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
	std::vector<Item*> itemShopList;
	bool drawChestText = false;
	pugi::xml_document textsParameters;
	std::map<SDL_Texture*, int> itemsList;
	pugi::char_t* searchText;
};