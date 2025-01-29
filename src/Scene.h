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
#include "Coin.h"
#include "Door.h"
#include "Events.h"
#include "NPC.h"
#include "Boss.h"

struct SDL_Texture;
class Events;

enum class GameState {
	MAINMENU, START, DEATH, PAUSESCREEN
};

enum class LEVELS {
	LEVEL0, CAVE, MOUNTAINS
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

	void CreateCoin(Vector2D pos, int quantity);

	void CreateAttack(EntityType type, Vector2D pos, bool directionLeft);

	void AddItem(int item);

	void DrawText(bool draw, pugi::char_t* dialogueSearch = const_cast<pugi::char_t*>("")) {
		drawChestText = draw;
		searchText = dialogueSearch;
	}

	// Called before quitting
	bool CleanUp();

	void LoadLevel(int lvl);

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

	DirectionPlayer GetPlayerDirection() {
		return player->GetDirection();
	}

	int GetPlayerCoins() {
		return player->GetCoins();
	}

	bool OnGuiMouseClickEvent(GuiControl* control);

	//check if an event is happening
	bool eventHappening = false;
	//manage events
	Events* eventManager = nullptr;

	bool regenItem = false;

private:

	bool quitGame = false;

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

	//level
	LEVELS currentLevel = LEVELS::LEVEL0;

	//L03: TODO 3b: Declare a Player attribute
	Player* player;
	GameState state;
	std::vector<Power*> fireballList;
	std::vector<Coin*> coinsList;
	std::vector<Enemy*> enemyList;
	std::vector<Chest*> chestList;
	std::vector<Merchant*> eventsList;
	std::vector<Item*> itemShopList;
	std::vector<Door*> doorList;
	std::vector<Boss*> bossList;
	bool drawChestText = false;
	pugi::xml_document textsDoc;
	pugi::xml_node textsParameters;
	std::map<SDL_Texture*, int> itemsList;
	pugi::char_t* searchText;
};