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
	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)(position.getX()), (int)(position.getY()), texH / 2, bodyType::DYNAMIC);
	pbody->ctype = ColliderType::ATTACKPLAYER;

	pbody->listener = this;

	// Set the gravity of the body
	pbody->body->SetGravityScale(0);

	return true;
}

bool Power::Update(float dt) {
	if (true) {
		//if (!Engine::GetInstance().scene.get()->IsPause()) {
		if (statePower == StatePower::DIE && currentAnimation->HasFinished()) colision = true;
		else if (statePower == StatePower::DIE) pbody->body->SetLinearVelocity({ 0, 0 });
		else {
			float speed = inverted ? -5.0f : 5.0f;
			pbody->body->SetLinearVelocity({ speed, 0 });
		}
	}
	else {
		pbody->body->SetLinearVelocity({ 0,0 });
		b2Transform pbodyPos = pbody->body->GetTransform();
		position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
		position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);
	}


	if (statePower == StatePower::IDLE) currentAnimation = &idle;


	b2Transform pbodyPos = pbody->body->GetTransform();


	if (inverted) position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texW / 2);
	else position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texW - texW / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH);

	Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), inverted ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE, &currentAnimation->GetCurrentFrame());
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

	if (statePower != StatePower::DIE && physB->ctype != ColliderType::SENSOR && physB->ctype != ColliderType::RANGE) {
		statePower = StatePower::DIE;
		currentAnimation = &explode;

		pbody->body->SetLinearVelocity({ 0, 0 });
		LOG("Fireball collided, starting explosion animation.");
	}
}

bool Power::HasCollision() {
	return colision;
}