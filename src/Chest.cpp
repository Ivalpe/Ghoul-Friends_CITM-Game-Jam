#include "Chest.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "Map.h"
#include "Pathfinding.h"

Chest::Chest() : Entity(EntityType::CHEST)
{

}

Chest::~Chest() {
}

bool Chest::Awake() {
	return true;
}

bool Chest::Start() {
	//Initilize textures
	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();

	//Load animations
	close.LoadAnimations(parameters.child("animations").child("closed"));
	open.LoadAnimations(parameters.child("animations").child("open"));
	currentAnimation = &close;

	//Sensor
	pbody = Engine::GetInstance().physics.get()->CreateCircleSensor((int)position.getX(), (int)position.getY() + texW, texW / 2, bodyType::STATIC);
	pbody->ctype = ColliderType::SENSOR;
	pbody->listener = this;

	return true;
}

bool Chest::Update(float dt) {

	pbody->body->SetLinearVelocity({ 0,0 });

	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

	Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), SDL_FLIP_NONE, &currentAnimation->GetCurrentFrame());

	currentAnimation->Update();

	if (openChest && Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F) == KEY_DOWN) {
		currentAnimation = &open;
		int i = rand() % 5 + 1;
		Engine::GetInstance().scene->AddItem(i);
		Engine::GetInstance().scene->DrawText(false);
	}

	return true;
}

bool Chest::CleanUp()
{
	Engine::GetInstance().textures.get()->UnLoad(texture);
	return true;
}

void Chest::SetPosition(Vector2D pos) {
	pos.setX(pos.getX() + texW / 2);
	pos.setY(pos.getY() + texH / 2);
	b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
	pbody->body->SetTransform(bodyPos, 0);
}

void Chest::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		if (currentAnimation == &close) {
			Engine::GetInstance().scene->DrawText(true);
			openChest = true;
		}
		break;
	default:
		break;
	}
}

void Chest::OnCollisionEnd(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		Engine::GetInstance().scene->DrawText(false);
		openChest = false;
		break;
	default:
		break;
	}
}