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

Scene::Scene() : Module()
{
	name = "scene";
	state = GameState::MAINMENU;
	textsParameters.load_file("texts.xml");
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

	//L08 Create a new item using the entity manager and set the position to (200, 672) to test
	Item* item = (Item*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM);
	item->position = Vector2D(200, 672);

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
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT) {

			std::vector<Vector2D> listEnemy = Engine::GetInstance().map->GetEnemyList();
			for (auto enemy : listEnemy) {
				Enemy* en = (Enemy*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY);
				en->SetParameters(configParameters.child("entities").child("enemies").child("skeleton"));
				en->SetEnemyType(EnemyType::SKELETON);
				en->Start();
				en->SetPosition({ enemy.getX(), enemy.getY() });
				enemyList.push_back(en);
			}

			std::vector<Vector2D> listChest = Engine::GetInstance().map->GetChestList();
			for (auto chest : listChest) {
				Chest* ch = (Chest*)Engine::GetInstance().entityManager->CreateEntity(EntityType::CHEST);
				ch->SetParameters(configParameters.child("entities").child("chest"));
				ch->Start();
				ch->SetPosition({ chest.getX(), chest.getY() });
				chestList.push_back(ch);
			}

			state = GameState::START;
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

	if (drawChestText) {
		std::string text = textsParameters.child("OpenChest").attribute("text").as_string();
		Engine::GetInstance().render->DrawText(text.c_str(), 60, 60, text.size() * 10, 40);
	}

	int x = 100;
	for (auto item : itemsList) {
		for (int i = 0; i < item.second; i++) {
			Engine::GetInstance().render->DrawTexture(item.first, x - Engine::GetInstance().render->camera.x / 4, 10 - Engine::GetInstance().render->camera.y / 4, SDL_FLIP_NONE);
			x += 20;
		}
	}

	return true;
}

void Scene::AddItem(int item) {
	if (item == 1) {
		auto it = itemsList.find(drums);
		if (it != itemsList.end()) {
			it->second++;
		}
		else {
			itemsList.emplace(drums, 1);
		}
	}
	else if (item == 2) {
		auto it = itemsList.find(armor);
		if (it != itemsList.end()) {
			it->second++;
		}
		else {
			itemsList.emplace(armor, 1);
		}
	}
	else if (item == 3) {
		auto it = itemsList.find(book);
		if (it != itemsList.end()) {
			it->second++;
		}
		else {
			itemsList.emplace(book, 1);
		}
	}
	else if (item == 4) {
		auto it = itemsList.find(life);
		if (it != itemsList.end()) {
			it->second++;
		}
		else {
			itemsList.emplace(life, 1);
		}
	}
	else if (item == 5) {
		auto it = itemsList.find(regeneration);
		if (it != itemsList.end()) {
			it->second++;
		}
		else {
			itemsList.emplace(regeneration, 1);
		}
	}
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
	if (player->GetDirection() == DirectionPlayer::LEFT) power->SetPosition({ playerPos.getX() - 4, playerPos.getY() + 14 });
	else power->SetPosition({ playerPos.getX() + 32, playerPos.getY() + 8 });

	fireballList.push_back(power);
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}
