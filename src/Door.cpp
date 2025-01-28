#include "Door.h"
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

Door::Door() : Entity(EntityType::DOOR)
{

}

Door::~Door() {
}

bool Door::Awake() {
	return true;
}

bool Door::Start() {

	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();

	//Load animations
	door.LoadAnimations(parameters.child("animations").child("idle"));
	currentAnimation = &door;

	//Sensor
	pbody = Engine::GetInstance().physics.get()->CreateCircleSensor((int)position.getX(), (int)position.getY(), texH / 2, bodyType::STATIC);
	pbody->ctype = ColliderType::SENSOR;
	pbody->listener = this;

	return true;
}

bool Door::Update(float dt) {

	pbody->body->SetLinearVelocity({ 0,0 });

	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

	Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX() + 8, (int)position.getY() - 16, SDL_FLIP_NONE);

	currentAnimation->Update();

	if (canDoor && Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F) == KEY_DOWN) {
		Engine::GetInstance().scene->LoadLevel(level);
	}
	return true;
}

bool Door::CleanUp()
{
	Engine::GetInstance().textures.get()->UnLoad(texture);
	return true;
}

void Door::SetPosition(Vector2D pos) {
	pos.setX(pos.getX() + texW / 2);
	pos.setY(pos.getY() + texH / 2);
	b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
	pbody->body->SetTransform(bodyPos, 0);
}

void Door::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		canDoor = true;
		Engine::GetInstance().scene->DrawText(true, const_cast<pugi::char_t*>("Caves"));
		break;
	default:
		break;
	}
}

void Door::OnCollisionEnd(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		canDoor = false;
		Engine::GetInstance().scene->DrawText(false);
		break;
	default:
		break;
	}
}