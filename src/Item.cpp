#include "Item.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"

Item::Item() : Entity(EntityType::ITEM)
{
	name = "item";
}

Item::~Item() {}

bool Item::Awake() {
	return true;
}

bool Item::Start() {

	Engine::GetInstance().textures.get()->GetSize(texture, texW, texH);

	//Sensor
	pbody = Engine::GetInstance().physics.get()->CreateCircleSensor((int)position.getX(), (int)position.getY() + texW, texW / 2, bodyType::STATIC);
	pbody->ctype = ColliderType::SENSOR;
	pbody->listener = this;

	return true;
}

void Item::SetTexture(int ran) {
	if (ran >= 0 && ran <= 19) { //Drums
		texture = Engine::GetInstance().textures.get()->Load("Assets/Items/drums.png");
	}
	else if (ran >= 20 && ran <= 39) { //Armor
		texture = Engine::GetInstance().textures.get()->Load("Assets/Items/armor.png");
	}
	else if (ran >= 40 && ran <= 59) { //Book
		texture = Engine::GetInstance().textures.get()->Load("Assets/Items/bookDmg.png");
	}
	else if (ran >= 60 && ran <= 79) { //Life
		texture = Engine::GetInstance().textures.get()->Load("Assets/Items/life.png");
	}
	else if (ran >= 80 && ran <= 100) { //Regeneration
		texture = Engine::GetInstance().textures.get()->Load("Assets/Items/regeneration.png");
	}
	item = ran;
}

bool Item::Update(float dt)
{
	// L08 TODO 4: Add a physics to an item - update the position of the object from the physics.  

	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

	Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), SDL_FLIP_NONE);

	if (canBuy && Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F) == KEY_DOWN) {
		Engine::GetInstance().scene->AddItem(item);
		buyed = true;
	}

	return true;
}

void Item::SetPosition(Vector2D pos) {
	pos.setX(pos.getX() + texW / 2);
	pos.setY(pos.getY() + texH / 2);
	b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
	pbody->body->SetTransform(bodyPos, 0);
}

bool Item::CleanUp()
{
	return true;
}

void Item::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		Engine::GetInstance().scene->DrawText(true, const_cast<pugi::char_t*>("BuyItem"));
		canBuy = true;
		break;
	default:
		break;
	}
}

void Item::OnCollisionEnd(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		canBuy = false;
		Engine::GetInstance().scene->DrawText(false);
		break;
	default:
		break;
	}
}