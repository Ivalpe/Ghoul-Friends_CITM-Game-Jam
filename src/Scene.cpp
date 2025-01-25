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

Scene::Scene() : Module()
{
	name = "scene";
	img = nullptr;
	state = GameState::MAINMENU;
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

	//L04: TODO 3b: Instantiate the player using the entity manager
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
	if (player->GetDirection() == DirectionPlayer::LEFT) power->SetPosition({ playerPos.getX() - 4, playerPos.getY() + 14 });
	else power->SetPosition({ playerPos.getX() + 32, playerPos.getY() + 8 });

	fireballList.push_back(power);
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	SDL_DestroyTexture(img);

	return true;
}
