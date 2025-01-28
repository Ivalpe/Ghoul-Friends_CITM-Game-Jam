#include "Coin.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"

Coin::Coin() : Entity(EntityType::COIN)
{
	name = "item";
}

Coin::~Coin() {}

bool Coin::Awake() {
	return true;
}

bool Coin::Start() {

	texture = Engine::GetInstance().textures.get()->Load("Assets/Items/coin.png");

	Engine::GetInstance().textures.get()->GetSize(texture, texW, texH);
	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() + texH / 2, (int)position.getY() + texH / 2, texH / 4, bodyType::DYNAMIC);

	pbody->ctype = ColliderType::COIN;
	pbody->listener = this;

	return true;
}

bool Coin::Update(float dt)
{

	b2Vec2 velocity = b2Vec2(0, -GRAVITY_Y / 10);

	pbody->body->SetLinearVelocity(velocity);

	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

	

	Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), SDL_FLIP_NONE);

	return true;
}

void Coin::SetPosition(Vector2D pos) {
	pos.setX(pos.getX() + texW / 2);
	pos.setY(pos.getY() + texH / 2);
	b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
	pbody->body->SetTransform(bodyPos, 0);
}

bool Coin::CleanUp()
{
	return true;
}

void Coin::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		collected = true;
		break;
	default:
		break;
	}
}

void Coin::OnCollisionEnd(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		break;
	default:
		break;
	}
}