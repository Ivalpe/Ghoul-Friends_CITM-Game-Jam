#include "Merchant.h"
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

Merchant::Merchant() : Entity(EntityType::ENEMY)
{

}

Merchant::~Merchant() {
}

bool Merchant::Awake() {
	return true;
}

bool Merchant::Start() {
	speed = 1.9f;

	//Initilize textures
	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();

	//Load animations
	idle.LoadAnimations(parameters.child("animations").child("idle"));
	sell.LoadAnimations(parameters.child("animations").child("sell"));
	currentAnimation = &idle;

	//Add a physics to an item - initialize the physics body
	pbody = Engine::GetInstance().physics.get()->CreateCircleSensor((int)position.getX(), (int)position.getY() + texW, texW / 2, bodyType::STATIC);

	//Assign collider type
	pbody->ctype = ColliderType::SENSOR;
	pbody->listener = this;

	//Sensor
	sensor = Engine::GetInstance().physics.get()->CreateCircleSensor((int)position.getX(), (int)position.getY() + texH, texW * 6, bodyType::KINEMATIC);
	sensor->ctype = ColliderType::RANGE;
	sensor->listener = this;

	// Set the gravity of the body
	if (!parameters.attribute("gravity").as_bool()) pbody->body->SetGravityScale(0);

	return true;
}

bool Merchant::Update(float dt) {

	velocity = b2Vec2(0, -GRAVITY_Y);

	pbody->body->SetLinearVelocity(velocity);

	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

	Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY() + 1, flipType, &currentAnimation->GetCurrentFrame());

	currentAnimation->Update();

	b2Vec2 enemyPos = pbody->body->GetPosition();
	sensor->body->SetTransform({ enemyPos.x, enemyPos.y }, 0);
	return true;
}

bool Merchant::CleanUp()
{
	Engine::GetInstance().textures.get()->UnLoad(texture);
	return true;
}

void Merchant::SetPosition(Vector2D pos) {
	pos.setX(pos.getX() + texW / 2);
	pos.setY(pos.getY() + texH / 2);
	b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
	pbody->body->SetTransform(bodyPos, 0);
}

Vector2D Merchant::GetPosition() {
	b2Vec2 bodyPos = pbody->body->GetTransform().p;
	Vector2D pos = Vector2D(METERS_TO_PIXELS(bodyPos.x), METERS_TO_PIXELS(bodyPos.y));
	return pos;
}

void Merchant::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		if (physA->ctype == ColliderType::RANGE)
			currentAnimation = &sell;
		break;
	default:
		break;
	}
}

void Merchant::OnCollisionEnd(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		if (physA->ctype == ColliderType::RANGE)
			currentAnimation = &idle;
		break;
	default:
		break;
	}
}