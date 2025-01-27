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

Scene::Scene() : Module()
{
	name = "scene";
	state = GameState::MAINMENU;
	textsParameters.load_file("texts.xml");
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

	std::vector<const char*> names = { "New Game", "Load Game", "Settings", "Credits" , "Exit Game" };
	int coordInitial = 360, interspace = 100;
	GuiControlButton* button;
	for (auto n : names) {
		button = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, Engine::GetInstance().uiManager.get()->GetSize(GuiClass::MAIN_MENU), n, { 480, coordInitial, 250,60 }, this, GuiClass::MAIN_MENU);
		button->SetTexture(buttonTexture, buttonTexture, buttonPressed, buttonTexture);
		Engine::GetInstance().uiManager.get()->Add(GuiClass::MAIN_MENU, button);
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
			Engine::GetInstance().physics->DeleteBody((*it)->getSensorBody());
			Engine::GetInstance().physics->DeleteBody((*it)->getRangeBody());
			Engine::GetInstance().physics->DeleteBody((*it)->getBody());
			Engine::GetInstance().entityManager->DestroyEntity(*it);
			it = enemyList.erase(it);
		}
		else ++it;
	}

	for (auto it = itemShopList.begin(); it != itemShopList.end();) {
		if ((*it)->isBuyed()) {
			Engine::GetInstance().physics->DeleteBody((*it)->getBody());
			Engine::GetInstance().entityManager->DestroyEntity(*it);
			it = itemShopList.erase(it);
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

	switch (control->id)
	{
	case 1:
		listEnemy = Engine::GetInstance().map->GetEnemyList();
		for (auto enemy : listEnemy) {
			Enemy* en = (Enemy*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY);
			en->SetParameters(configParameters.child("entities").child("enemies").child("skeleton"));
			en->SetEnemyType(EnemyType::SKELETON);
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

		Engine::GetInstance().uiManager->Show(GuiClass::MAIN_MENU, false);
		state = GameState::START;
		break;
	default:
		break;
	}
	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

void Scene::CreateAttack() {
	Power* power = (Power*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ATTACKPLAYER);
	power->SetParameters(configParameters.child("entities").child("fireball"));
	if (player->GetDirection() == DirectionPlayer::LEFT) power->Start(true);
	else power->Start(false);

	Vector2D playerPos = player->GetPosition();
	if (player->GetDirection() == DirectionPlayer::LEFT) power->SetPosition({ playerPos.getX() - 16, playerPos.getY() });
	else power->SetPosition({ playerPos.getX() + 20, playerPos.getY() });

	fireballList.push_back(power);
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}
