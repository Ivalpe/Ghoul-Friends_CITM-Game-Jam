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
#include "Boss.h"

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

	eventManager = new Events();
	eventManager->SetEventParameters(configParameters.child("entities").child("events"));

	return ret;
}

void Scene::PlayAudio(int audio) {
	switch (audio) {
	case 1:
		Engine::GetInstance().audio.get()->PlayFx(audioBow);
		break;
	case 2:
		Engine::GetInstance().audio.get()->PlayFx(audioBowHit);
		break;
	case 3:
		Engine::GetInstance().audio.get()->PlayFx(jumpPlayer);
		break;
	case 4:
		Engine::GetInstance().audio.get()->PlayFx(spiderDeath);
		break;
	case 5:
		Engine::GetInstance().audio.get()->PlayFx(fireball);
		break;
	case 6:
		Engine::GetInstance().audio.get()->PlayFx(swordSwing);
		break;
	}
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
	imgFinal = Engine::GetInstance().textures.get()->Load("Assets/Menus/Final.png");

	audioBow = Engine::GetInstance().audio.get()->LoadFx(configParameters.child("audio").child("audioBow").attribute("path").as_string());
	audioBowHit = Engine::GetInstance().audio.get()->LoadFx(configParameters.child("audio").child("audioBowHit").attribute("path").as_string());
	jumpPlayer = Engine::GetInstance().audio.get()->LoadFx(configParameters.child("audio").child("jumpPlayer").attribute("path").as_string());
	spiderDeath = Engine::GetInstance().audio.get()->LoadFx(configParameters.child("audio").child("spiderDeath").attribute("path").as_string());
	swordSwing = Engine::GetInstance().audio.get()->LoadFx(configParameters.child("audio").child("sword").attribute("path").as_string());

	fireball = Engine::GetInstance().audio.get()->LoadFx(configParameters.child("audio").child("fireball").attribute("path").as_string());

	const char* newGame = textsParameters.child("NewGame").attribute("text").as_string();
	const char* settings = textsParameters.child("Settings").attribute("text").as_string();
	const char* exit = textsParameters.child("Exit").attribute("text").as_string();
	const char* resume = textsParameters.child("Continue").attribute("text").as_string();
	const char* return2Menu = textsParameters.child("Return").attribute("text").as_string();

	std::vector<const char*> names = { newGame, exit };
	int coordInitial = 480, interspace = 120;
	GuiControlButton* button;
	for (auto n : names) {
		button = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, Engine::GetInstance().uiManager.get()->GetSize(GuiClass::MAIN_MENU), n, { 630, coordInitial, 250,60 }, this, GuiClass::MAIN_MENU);
		button->SetTexture(buttonTexture, buttonTexture, buttonPressed, buttonTexture);
		Engine::GetInstance().uiManager.get()->Add(GuiClass::MAIN_MENU, button);
		coordInitial += interspace;
	}

	coordInitial = 380, interspace = 120;
	names = { resume, return2Menu, exit };
	for (auto n : names) {
		button = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, Engine::GetInstance().uiManager.get()->GetSize(GuiClass::PAUSE), n, { 800, coordInitial, 250,60 }, this, GuiClass::MAIN_MENU);
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

	for (auto b : bossList) {
		Engine::GetInstance().physics->DeleteBody((b)->getLeftSensorBody());
		Engine::GetInstance().physics->DeleteBody((b)->getRightSensorBody());
		Engine::GetInstance().physics->DeleteBody((b)->getRangeBody());
		Engine::GetInstance().physics->DeleteBody((b)->getActiveBody());
		Engine::GetInstance().physics->DeleteBody((b)->getBody());
		Engine::GetInstance().entityManager->DestroyEntity(b);
	}
	bossList.clear();

	for (auto c : chestList) {
		Engine::GetInstance().physics->DeleteBody((c)->getBody());
		Engine::GetInstance().entityManager->DestroyEntity(c);
	}
	chestList.clear();

	for (auto m : eventsList) {
		Engine::GetInstance().physics->DeleteBody((m)->getSensorBody());
		Engine::GetInstance().physics->DeleteBody((m)->getBody());
		Engine::GetInstance().entityManager->DestroyEntity(m);
	}
	eventsList.clear();

	for (auto i : itemShopList) {
		Engine::GetInstance().physics->DeleteBody((i)->getBody());
		Engine::GetInstance().entityManager->DestroyEntity(i);
	}
	itemShopList.clear();

	for (auto i : doorList) {
		i->dontRender();
	}

	eventManager->CleanUp();
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
		player->SetPosition({ 184, 1000 });
		currentLevel = LEVELS::MOUNTAINS;
		break;
	default:
		break;
	}

	eventManager->LoadLevelEvents(lvl);

	std::vector<Vector2D> listEnemy, listChest, listEvents;
	std::map<int, Vector2D> listDoorMap, listBosses;

	listEnemy = Engine::GetInstance().map->GetEnemyList();
	for (auto enemy : listEnemy) {
		Enemy* en;
		if (currentLevel == LEVELS::LEVEL0) {
			int ran = rand() % 10 + 1;
			en = (Enemy*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY);
			if (ran <= 5) {
				en->SetParameters(configParameters.child("entities").child("enemies").child("skeleton"));
				en->SetEnemyType(EnemyType::SKELETON);
			}
			else {
				en->SetParameters(configParameters.child("entities").child("enemies").child("skeletonArcher"));
				en->SetEnemyType(EnemyType::SKELETON_ARCHER);
			}
		}
		else if (currentLevel == LEVELS::CAVE) {
			en = (Enemy*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY);
			en->SetParameters(configParameters.child("entities").child("enemies").child("spider"));
			en->SetEnemyType(EnemyType::SPIDER);
		}
		else {
			en = (Enemy*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY);
			en->SetParameters(configParameters.child("entities").child("enemies").child("demon"));
			en->SetEnemyType(EnemyType::DEMON);
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

	listBosses = Engine::GetInstance().map->GetBossList();
	for (auto boss : listBosses) {
		Boss* b = (Boss*)Engine::GetInstance().entityManager->CreateEntity(EntityType::BOSS);
		b->SetParameters(configParameters.child("entities").child("bosses").child("bossMountain"));
		b->Start();
		b->SetPosition({ boss.second.getX(), boss.second.getY() });
		bossList.push_back(b);
	}

	menuMusic = false;
	switch (lvl) {
	case 0:
		Engine::GetInstance().audio.get()->PlayMusic("Assets/Audio/Music/1 Exploration LOOP TomMusic.ogg");
		break;
	case 1:
		Engine::GetInstance().audio.get()->PlayMusic("Assets/Audio/Music/9 Searching Further LOOP TomMusic.ogg");
		break;
	case 2:
		Engine::GetInstance().audio.get()->PlayMusic("Assets/Audio/Music/5 A Safe Space LOOP TomMusic.ogg");
		break;
	}
}

void Scene::PlayBossMusic() {
	if (!bossMusic) {
		Engine::GetInstance().audio.get()->PlayMusic("Assets/Audio/Music/7 The Wilderness LOOP TomMusic.ogg");
		bossMusic = true;
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
		if (!menuMusic) {
			Engine::GetInstance().audio.get()->PlayMusic("Assets/Audio/Music/10 Terminus FULL LOOP TomMusic.ogg");
			menuMusic = true;
		}
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
		if (deathTime >= deathMaxTime) {
			LoadLevel(0);
			state = GameState::START;
			player->startRespawn = true;
		}
		break;
	case GameState::FINISH:
		Engine::GetInstance().render.get()->DrawTexture(imgFinal, 0, 0, SDL_FLIP_NONE, NULL, false, false);
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) quitGame = true;
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

	if (drawFinish)
		state = GameState::FINISH;

	if (drawChestText) {
		std::string text = textsParameters.child(searchText).attribute("text").as_string();
		Engine::GetInstance().render->DrawText(text.c_str(), 1920 / 2, 768 / 2 + 32, text.size() * 10, 40);
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
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F5) == KEY_DOWN) {
		AddItem(41);
	}
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F9) == KEY_DOWN) {
		player->CanShoot(true);
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
	else if (item >= 40 && item <= 59) {
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
		regenItem = true;
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
	std::map<int, Vector2D> listDoorMap, listBosses;
	int level;

	switch (state) {
	case GameState::MAINMENU:
		switch (control->id)
		{
		case 1:
			itemsList.clear();
			eventManager->FullReset();
			LoadLevel(0);
			player->Restart();

			Engine::GetInstance().uiManager->Show(GuiClass::MAIN_MENU, false);
			state = GameState::START;
			break;
		case 2:
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

	eventManager->Update();

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


void Scene::CreateAttack(EntityType type, Vector2D pos, bool directionLeft, b2Vec2 speed) {
	Power* power = (Power*)Engine::GetInstance().entityManager->CreateEntity(type);
	if (type == EntityType::ATTACKPLAYER) {
		PlayAudio(1);
		power->SetParameters(configParameters.child("entities").child("arrow"), TypePower::ATTACKPLAYER);
	}
	else if (type == EntityType::ARROW) {
		PlayAudio(1);
		power->SetParameters(configParameters.child("entities").child("arrow"), TypePower::ARROW);
	}
	else if (type == EntityType::BOSSTRIDENT)
		power->SetParameters(configParameters.child("entities").child("bossTrident"), TypePower::ARROW);
	else if (type == EntityType::FIRESHOOTER)
		power->SetParameters(configParameters.child("entities").child("fireShooter"), TypePower::FIRESHOOTER);
	else if (type == EntityType::FIREBALL) {
		PlayAudio(5);
		power->SetParameters(configParameters.child("entities").child("fireball"), TypePower::FIREBALL);
		power->SetSpeed(speed);
	}

	if (directionLeft) power->Start(true);
	else power->Start(false);

	if (type == EntityType::ATTACKPLAYER || type == EntityType::ARROW) {
		if (directionLeft) power->SetPosition({ pos.getX() - 16, pos.getY() + 2 });
		else power->SetPosition({ pos.getX() + 24, pos.getY() });
	}
	else if (type == EntityType::BOSSTRIDENT) {
		if (directionLeft) power->SetPosition({ pos.getX() - 30, pos.getY() + 16 });
		else power->SetPosition({ pos.getX() + 30, pos.getY() + 16 });
	}
	else if (type == EntityType::FIRESHOOTER) {
		if (directionLeft) power->SetPosition({ pos.getX() + 24, pos.getY() - 14 });
		else power->SetPosition({ pos.getX() + 8, pos.getY() - 14 });
	}
	else if (type == EntityType::FIREBALL) {
		if (directionLeft) power->SetPosition({ pos.getX() + 16, pos.getY() + 32 });
		else power->SetPosition({ pos.getX() + 16, pos.getY() + 32 });
	}


	fireballList.push_back(power);
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}
