#include "Power.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "Map.h"

Power::Power(EntityType et) : Entity(et) {
}

Power::~Power() {
}

bool Power::Awake() {
	df = DirectionPower::RIGHT;
	return true;
}

bool Power::Start(bool inv) {

	inverted = inv;
	colision = false;
	//initilize textures
	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();

	//Load animations


	idle.LoadAnimations(parameters.child("animations").child("idle"));
	explode.LoadAnimations(parameters.child("animations").child("explode"));
	fire.LoadAnimations(parameters.child("animations").child("fire"));
	currentAnimation = &idle;

	//Assign collider type
	if (typePower == TypePower::FIRESHOOTER) {
		pbody = Engine::GetInstance().physics.get()->CreateCircleSensor((int)(position.getX()), (int)(position.getY()), texH / 3, bodyType::DYNAMIC);
		pbody->ctype = ColliderType::ATTACKENEMY;
	}
	else if (typePower == TypePower::ARROW) {
		pbody = Engine::GetInstance().physics.get()->CreateCircle((int)(position.getX()), (int)(position.getY()), texH / 3, bodyType::DYNAMIC);
		pbody->ctype = ColliderType::ATTACKPLAYER;
	}
	else {
		pbody = Engine::GetInstance().physics.get()->CreateCircle((int)(position.getX()), (int)(position.getY()), texH / 3, bodyType::DYNAMIC);
		pbody->ctype = ColliderType::ATTACKENEMY;
	}


	pbody->listener = this;

	//get life and damage values
	damage = parameters.attribute("damage").as_int();

	//Assign damage that enemy does
	pbody->damageDone = damage;

	// Set the gravity of the body
	pbody->body->SetGravityScale(0);

	switch (typePower)
	{
	case TypePower::FIREBALL:
		maxTime = 120;
		break;
	case TypePower::FIRESHOOTER:
		maxTime = 240;
		break;
	}

	return true;
}

bool Power::Update(float dt) {
	switch (typePower)
	{
	case TypePower::ARROW:
		if (true) {
			//if (!Engine::GetInstance().scene.get()->IsPause()) {
			if (statePower == StatePower::DIE && currentAnimation->HasFinished()) colision = true;
			else if (statePower == StatePower::DIE) pbody->body->SetLinearVelocity({ 0, 0 });
			else {
				float speed = inverted ? -10.0f : 10.0f;
				pbody->body->SetLinearVelocity({ speed, 0 });
			}
		}
		else {
			pbody->body->SetLinearVelocity({ 0,0 });
			b2Transform pbodyPos = pbody->body->GetTransform();
			position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
			position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);
		}
		break;
	case TypePower::FIRESHOOTER:
		if (statePower == StatePower::DIE && currentAnimation->HasFinished()) colision = true;
		pbody->body->SetLinearVelocity({ 0,0 });
		coolShoot++;
		if (coolShoot == 60) {
			Engine::GetInstance().scene->CreateAttack(EntityType::FIREBALL, position, inverted, speed);
			if (inverted) speed.Set(speed.x - 1, speed.y - 1);
			else  speed.Set(speed.x + 1, speed.y + 1);
			coolShoot = 0;
		}
		break;
	case TypePower::FIREBALL:
		if (statePower == StatePower::DIE && currentAnimation->HasFinished()) colision = true;
		else if (statePower == StatePower::DIE) pbody->body->SetLinearVelocity({ 0, 0 });
		else {
			pbody->body->SetLinearVelocity(speedFireball);
		}
		break;
	default:
		break;
	}



	if (statePower == StatePower::IDLE) currentAnimation = &idle;

	maxTime--;

	if (maxTime <= 0) {
		currentAnimation = &explode;
		statePower = StatePower::DIE;
	}


	b2Transform pbodyPos = pbody->body->GetTransform();


	if (inverted) position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texW / 2);
	else position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texW - texW / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH);

	Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX() + (inverted ? 0 : 16), (int)position.getY() + 8, inverted ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE, &currentAnimation->GetCurrentFrame());
	currentAnimation->Update();

	return true;
}

bool Power::CleanUp()
{
	return true;
}

void Power::SetPosition(Vector2D pos) {
	float offsetX = inverted ? -texW / 2 : texW / 2;
	pos.setX(pos.getX() + offsetX);
	pos.setY(pos.getY() + texH / 2);

	b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
	pbody->body->SetTransform(bodyPos, 0);
}

Vector2D Power::GetPosition() {
	b2Vec2 bodyPos = pbody->body->GetTransform().p;
	Vector2D pos = Vector2D(METERS_TO_PIXELS(bodyPos.x), METERS_TO_PIXELS(bodyPos.y));
	return pos;
}
void Power::OnCollision(PhysBody* physA, PhysBody* physB) {

	if (statePower != StatePower::DIE && physB->ctype != ColliderType::SENSOR && physB->ctype != ColliderType::RANGE
		&& physB->ctype != ColliderType::MERCHANT && physB->ctype != ColliderType::RANGELEFT && physB->ctype != ColliderType::RANGERIGHT
		&& physB->ctype != ColliderType::ACTIVEBOSS &&
		((pbody->ctype == ColliderType::ATTACKENEMY && physB->ctype != ColliderType::ATTACKENEMY) ||
			(pbody->ctype == ColliderType::ATTACKPLAYER && physB->ctype != ColliderType::ATTACKPLAYER))) {
		statePower = StatePower::DIE;
		currentAnimation = &explode;

		pbody->body->SetLinearVelocity({ 0, 0 });
		LOG("Fireball collided, starting explosion animation.");
	}
}

bool Power::HasCollision() {
	return colision;
}