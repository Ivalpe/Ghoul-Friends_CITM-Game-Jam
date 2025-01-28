#include "Engine.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "Log.h"
#include "Entity.h"
#include "Physics.h"
#include "EntityManager.h"
#include "Player.h"
#include "Map.h"
#include "Item.h"
#include "Power.h"
#include "Enemy.h"
#include "Chest.h"
#include "Merchant.h"
#include "GuiControlButton.h"
#include "Coin.h"
#include "Door.h"
#include <map>

Scene::Scene() : Module()
{
	name = "scene";
	state = GameState::MAINMENU;
	textsDoc.load_file("texts.xml");
	textsParameters = textsDoc.child("texts");

	srand(time(NULL));
}

// Destructor
Scene::~Scene()
{
}

// Called before render is available
bool Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	player = (Player*)Engine::GetInstance().entityManager->CreateEntity(EntityType::PLAYER);
	player->SetParameters(configParameters.child("entities").child("player"));

	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	Engine::GetInstance().map->Load("Assets/Maps/", "Level0.tmx");

	drums = Engine::GetInstance().textures.get()->Load("Assets/Items/drums.png");
	book = Engine::GetInstance().textures.get()->Load("Assets/Items/bookDmg.png");
	armor = Engine::GetInstance().textures.get()->Load("Assets/Items/armor.png");
	life = Engine::GetInstance().textures.get()->Load("Assets/Items/life.png");
	regeneration = Engine::GetInstance().textures.get()->Load("Assets/Items/regeneration.png");
	buttonTexture = Engine::GetInstance().textures.get()->Load("Assets/Menus/Button.png");
	buttonPressed = Engine::GetInstance().textures.get()->Load("Assets/Menus/ButtonPressed.png");
	buttonShop = Engine::GetInstance().textures.get()->Load("Assets/Menus/ButtonShop.png");

	const char* newGame = textsParameters.child("NewGame").attribute("text").as_string();
	const char* settings = textsParameters.child("Settings").attribute("text").as_string();
	const char* exit = textsParameters.child("Exit").attribute("text").as_string();
	const char* resume = textsParameters.child("Continue").attribute("text").as_string();
	const char* return2Menu = textsParameters.child("Return").attribute("text").as_string();

	std::vector<const char*> names = { newGame, settings, exit };
	int coordInitial = 360, interspace = 100;
	GuiControlButton* button;
	for (auto n : names) {
		button = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, Engine::GetInstance().uiManager.get()->GetSize(GuiClass::MAIN_MENU), n, { 480, coordInitial, 250,60 }, this, GuiClass::MAIN_MENU);
		button->SetTexture(buttonTexture, buttonTexture, buttonPressed, buttonTexture);
		Engine::GetInstance().uiManager.get()->Add(GuiClass::MAIN_MENU, button);
		coordInitial += interspace;
	}

	coordInitial = 500, interspace = 100;
	names = { resume, return2Menu, exit };
	for (auto n : names) {
		button = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, Engine::GetInstance().uiManager.get()->GetSize(GuiClass::PAUSE), n, { 600, coordInitial, 250,60 }, this, GuiClass::MAIN_MENU);
		button->SetTexture(buttonTexture, buttonTexture, buttonPressed, buttonTexture);
		Engine::GetInstance().uiManager.get()->Add(GuiClass::PAUSE, button);
		coordInitial += interspace;
	}

	Engine::GetInstance().uiManager.get()->Show(GuiClass::MAIN_MENU, true);

	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	return true;
}

void Scene::LoadLevel(int lvl) {

	//clear lists
	for (auto f : fireballList) {
		f->CleanUp();
		Engine::GetInstance().physics->DeleteBody((f)->getBody());
		Engine::GetInstance().entityManager->DestroyEntity(f);
	}
	fireballList.clear();


	for (auto c : coinsList) {
		Engine::GetInstance().physics->DeleteBody((c)->getBody());
		Engine::GetInstance().entityManager->DestroyEntity(c);
	}
	coinsList.clear();

	for (auto e : enemyList) {
		Engine::GetInstance().physics->DeleteBody((e)->getSensorBody());
		Engine::GetInstance().physics->DeleteBody((e)->getRangeBody());
		Engine::GetInstance().physics->DeleteBody((e)->getBody());
		Engine::GetInstance().entityManager->DestroyEntity(e);
	}
	enemyList.clear();

	for (auto c : chestList) {
		Engine::GetInstance().physics->DeleteBody((c)->getBody());
		Engine::GetInstance().entityManager->DestroyEntity(c);
	}
	chestList.clear();

	Engine::GetInstance().map->NewLevelCleanUp();

	switch (lvl)
	{
	case 0:
		Engine::GetInstance().map->Load("Assets/Maps/", "Level0.tmx");
		player->SetPosition({ 89, 264 });
		currentLevel = LEVELS::LEVEL0;
		break;
	case 1:
		Engine::GetInstance().map->Load("Assets/Maps/", "Cave.tmx");
		player->SetPosition({ 320, 800 });
		currentLevel = LEVELS::CAVE;
		break;
	case 2:
		Engine::GetInstance().map->Load("Assets/Maps/", "Mountain.tmx");
		player->SetPosition({ 320, 800 });
		currentLevel = LEVELS::MOUNTAINS;
		break;
	default:
		break;
	}

	std::vector<Vector2D> listEnemy, listChest, listEvents;
	std::map<int, Vector2D> listDoorMap;

	listEnemy = Engine::GetInstance().map->GetEnemyList();
	for (auto enemy : listEnemy) {
		int ran = rand() % 10 + 1;
		Enemy* en = (Enemy*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY);
		if (ran <= 5) {
			en->SetParameters(configParameters.child("entities").child("enemies").child("skeleton"));
			en->SetEnemyType(EnemyType::SKELETON);
		}
		else {
			en->SetParameters(configParameters.child("entities").child("enemies").child("skeletonArcher"));
			en->SetEnemyType(EnemyType::SKELETON_ARCHER);
		}
		en->Start();
		en->SetPosition({ enemy.getX(), enemy.getY() });
		enemyList.push_back(en);
	}

	listChest = Engine::GetInstance().map->GetChestList();
	for (auto chest : listChest) {
		Chest* ch = (Chest*)Engine::GetInstance().entityManager->CreateEntity(EntityType::CHEST);
		ch->SetParameters(configParameters.child("entities").child("chest"));
		ch->Start();
		ch->SetPosition({ chest.getX(), chest.getY() });
		chestList.push_back(ch);
	}

	listEvents = Engine::GetInstance().map->GetRandomEventList();
	for (auto event : listEvents) {
		Merchant* mc = (Merchant*)Engine::GetInstance().entityManager->CreateEntity(EntityType::MERCHANT);
		mc->SetParameters(configParameters.child("entities").child("merchant"));
		mc->Start();
		mc->SetPosition({ event.getX(), event.getY() - 16 });
		eventsList.push_back(mc);
		Item* i = (Item*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM);
		int ran = rand() % 100 + 1;
		i->SetTexture(ran);
		i->Start();
		i->SetPosition({ event.getX() - 16, event.getY() });
		itemShopList.push_back(i);
	}

	listDoorMap = Engine::GetInstance().map->GetDoorList();
	for (auto door : listDoorMap) {
		Door* d = (Door*)Engine::GetInstance().entityManager->CreateEntity(EntityType::DOOR);
		d->SetParameters(configParameters.child("entities").child("doorCave"), door.first);
		d->Start();
		d->SetPosition({ door.second.getX(), door.second.getY() });
		doorList.push_back(d);
	}
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	float camSpeed = 1;
	int cameraX, cameraY;
	int cameraMaxX, cameraMaxY;

	switch (state)
	{
	case GameState::MAINMENU:
		break;
	case GameState::START:
		Engine::GetInstance().render.get()->camera.x = ((player->position.getX() * -1) + 200) * 4;
		Engine::GetInstance().render.get()->camera.y = ((player->position.getY() * -1) + 100) * 4;

		cameraX = Engine::GetInstance().render.get()->camera.x;
		cameraY = Engine::GetInstance().render.get()->camera.y;
		cameraMaxX = Engine::GetInstance().map.get()->GetWidth() * 4 * 16 * -1 + 1920;
		cameraMaxY = Engine::GetInstance().map.get()->GetHeight() * 4 * 16 * -1 + 1080;
		if (cameraX >= 0) Engine::GetInstance().render.get()->camera.x = 0;
		if (cameraX <= cameraMaxX) Engine::GetInstance().render.get()->camera.x = cameraMaxX;
		if (cameraY >= 0) Engine::GetInstance().render.get()->camera.y = 0;
		if (cameraY <= cameraMaxY) Engine::GetInstance().render.get()->camera.y = cameraMaxY;

		if (player->hasDied and !player->startRespawn) {
			state = GameState::DEATH;
			deathTime = 0;
		}

		break;
	case GameState::DEATH:
		++deathTime;
		if (currentLevel != LEVELS::LEVEL0) LoadLevel(0);
		if (deathTime >= deathMaxTime) {
			state = GameState::START;
			player->startRespawn = true;
		}
		break;
	default:
		break;
	}

	for (auto it = fireballList.begin(); it != fireballList.end(); ) {
		if ((*it)->HasCollision()) {
			Engine::GetInstance().physics->DeleteBody((*it)->getBody());
			Engine::GetInstance().entityManager->DestroyEntity(*it);
			it = fireballList.erase(it);
		}
		else ++it;
	}

	// Clear dead enemies from the list
	for (auto it = enemyList.begin(); it != enemyList.end();) {
		if ((*it)->IsDead()) {
			int ran = rand() % 3 + 1;
			CreateCoin((*it)->GetPosition(), ran);
			Engine::GetInstance().physics->DeleteBody((*it)->getSensorBody());
			Engine::GetInstance().physics->DeleteBody((*it)->getRangeBody());
			Engine::GetInstance().physics->DeleteBody((*it)->getBody());
			Engine::GetInstance().entityManager->DestroyEntity(*it);
			it = enemyList.erase(it);
		}
		else ++it;
	}

	// Clear items buyed
	for (auto it = itemShopList.begin(); it != itemShopList.end();) {
		if ((*it)->isBuyed()) {
			Engine::GetInstance().physics->DeleteBody((*it)->getBody());
			Engine::GetInstance().entityManager->DestroyEntity(*it);
			it = itemShopList.erase(it);
		}
		else ++it;
	}

	// Clear coins collected
	for (auto it = coinsList.begin(); it != coinsList.end();) {
		if ((*it)->isCollected()) {
			Engine::GetInstance().physics->DeleteBody((*it)->getBody());
			Engine::GetInstance().entityManager->DestroyEntity(*it);
			it = coinsList.erase(it);
			player->AddCoins(10);
		}
		else ++it;
	}

	if (drawChestText) {
		std::string text = textsParameters.child(searchText).attribute("text").as_string();
		Engine::GetInstance().render->DrawText(text.c_str(), 60, 60, text.size() * 10, 40);
	}

	int x = 100;
	for (auto item : itemsList) {
		for (int i = 0; i < item.second; i++) {
			Engine::GetInstance().render->DrawTexture(item.first, x - Engine::GetInstance().render->camera.x / 4, 10 - Engine::GetInstance().render->camera.y / 4, SDL_FLIP_NONE);
			x += 20;
		}
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_L) == KEY_DOWN) {
		LoadLevel(1);
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) {
		AddItem(61);
	}
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F3) == KEY_DOWN) {
		AddItem(81);
	}
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F4) == KEY_DOWN) {
		AddItem(21);
	}

	return true;
}

void Scene::AddItem(int item) {

	if (item >= 0 && item <= 19) {
		auto it = itemsList.find(drums);
		if (it != itemsList.end()) {
			it->second++;
		}
		else {
			itemsList.emplace(drums, 1);
		}
	}
	else if (item >= 20 && item <= 39) {
		auto it = itemsList.find(armor);
		if (it != itemsList.end()) {
			it->second++;
		}
		else {
			itemsList.emplace(armor, 1);
		}
	}
	else if (item >= 30 && item <= 59) {
		auto it = itemsList.find(book);
		if (it != itemsList.end()) {
			it->second++;
		}
		else {
			itemsList.emplace(book, 1);
		}
	}
	else if (item >= 60 && item <= 79) {
		auto it = itemsList.find(life);
		if (it != itemsList.end()) {
			it->second++;
		}
		else {
			itemsList.emplace(life, 1);
		}
	}
	else if (item >= 80 && item <= 100) {
		auto it = itemsList.find(regeneration);
		if (it != itemsList.end()) {
			it->second++;
		}
		else {
			itemsList.emplace(regeneration, 1);
		}
	}

	player->AddItem(item);
}

bool Scene::OnGuiMouseClickEvent(GuiControl* control)
{
	std::vector<Vector2D> listEnemy, listChest, listEvents;
	std::map<int, Vector2D> listDoorMap;
	int level;

	switch (state) {
	case GameState::MAINMENU:
		switch (control->id)
		{
		case 1:
			listEnemy = Engine::GetInstance().map->GetEnemyList();
			for (auto enemy : listEnemy) {
				int ran = rand() % 10 + 1;
				Enemy* en = (Enemy*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY);
				if (ran <= 5) {
					en->SetParameters(configParameters.child("entities").child("enemies").child("skeleton"));
					en->SetEnemyType(EnemyType::SKELETON);
				}
				else {
					en->SetParameters(configParameters.child("entities").child("enemies").child("skeletonArcher"));
					en->SetEnemyType(EnemyType::SKELETON_ARCHER);
				}
				en->Start();
				en->SetPosition({ enemy.getX(), enemy.getY() });
				enemyList.push_back(en);
			}

			listChest = Engine::GetInstance().map->GetChestList();
			for (auto chest : listChest) {
				Chest* ch = (Chest*)Engine::GetInstance().entityManager->CreateEntity(EntityType::CHEST);
				ch->SetParameters(configParameters.child("entities").child("chest"));
				ch->Start();
				ch->SetPosition({ chest.getX(), chest.getY() });
				chestList.push_back(ch);
			}

			listEvents = Engine::GetInstance().map->GetRandomEventList();
			for (auto event : listEvents) {
				Merchant* mc = (Merchant*)Engine::GetInstance().entityManager->CreateEntity(EntityType::MERCHANT);
				mc->SetParameters(configParameters.child("entities").child("merchant"));
				mc->Start();
				mc->SetPosition({ event.getX(), event.getY() - 16 });
				eventsList.push_back(mc);
				Item* i = (Item*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM);
				int ran = rand() % 100 + 1;
				i->SetTexture(ran);
				i->Start();
				i->SetPosition({ event.getX() - 16, event.getY() });
				itemShopList.push_back(i);
			}

			listDoorMap = Engine::GetInstance().map->GetDoorList();
			for (auto door : listDoorMap) {
				Door* d = (Door*)Engine::GetInstance().entityManager->CreateEntity(EntityType::DOOR);
				d->SetParameters(configParameters.child("entities").child("doorCave"), door.first);
				d->Start();
				d->SetPosition({ door.second.getX(), door.second.getY() });
				doorList.push_back(d);
			}

			Engine::GetInstance().uiManager->Show(GuiClass::MAIN_MENU, false);
			state = GameState::START;
			break;
		case 3:
			quitGame = true;
			break;
		default:
			break;
		}
		break;
	case GameState::PAUSESCREEN:
		switch (control->id) {
		case 1:
			Engine::GetInstance().uiManager->Show(GuiClass::PAUSE, false);
			state = GameState::START;
			break;
		case 2:
			Engine::GetInstance().uiManager->Show(GuiClass::PAUSE, false);
			Engine::GetInstance().uiManager->Show(GuiClass::MAIN_MENU, true);
			state = GameState::MAINMENU;
			break;
		case 3:
			quitGame = true;
			break;
		}
		break;
	}
	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if (quitGame) ret = false;
	else if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) {
		switch (state) {
		case GameState::MAINMENU:
			ret = false;
			break;
		case GameState::START:
			state = GameState::PAUSESCREEN;
			Engine::GetInstance().uiManager.get()->Show(GuiClass::PAUSE, true);
			break;
		}
	}

	return ret;
}

void Scene::CreateCoin(Vector2D pos, int quantity) {
	for (size_t i = 0; i < quantity; i++)
	{
		Coin* coin = (Coin*)Engine::GetInstance().entityManager->CreateEntity(EntityType::COIN);
		coin->Start();
		int ran = rand() % 8 - 4;
		int ran2 = rand() % 3 + 1;
		pos.setX(pos.getX() + ran);
		pos.setY(pos.getY() - ran2);
		coin->SetPosition(pos);
		coinsList.push_back(coin);
	}
}


void Scene::CreateAttack(EntityType type, Vector2D pos, bool directionLeft) {
	Power* power = (Power*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ATTACKPLAYER);
	power->SetParameters(configParameters.child("entities").child("arrow"));
	if (directionLeft) power->Start(true);
	else power->Start(false);

	if (directionLeft) power->SetPosition({ pos.getX() - 16, pos.getY() + 2 });
	else power->SetPosition({ pos.getX() + 20, pos.getY() + 2 });

	fireballList.push_back(power);
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}
