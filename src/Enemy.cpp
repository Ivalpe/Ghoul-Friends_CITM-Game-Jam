#include "Enemy.h"
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

Enemy::Enemy() : Entity(EntityType::ENEMY)
{

}

Enemy::~Enemy() {
}

bool Enemy::Awake() {
	return true;
}

bool Enemy::Start() {
	tempChangeAnimation = 120;
	followPlayer = false;
	rangePlayer = false;
	speed = 1.9f;

	//Initilize textures
	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();

	//Load animations
	idle.LoadAnimations(parameters.child("animations").child("idle"));
	walk.LoadAnimations(parameters.child("animations").child("walk"));
	die.LoadAnimations(parameters.child("animations").child("die"));
	crouch.LoadAnimations(parameters.child("animations").child("crouch"));
	attack.LoadAnimations(parameters.child("animations").child("attack"));
	dmg.LoadAnimations(parameters.child("animations").child("dmg"));
	currentAnimation = &idle;

	//Add a physics to an item - initialize the physics body
	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX(), (int)position.getY() + texW, texW / 2, bodyType::DYNAMIC);

	//Assign collider type
	pbody->ctype = ColliderType::SKELETON;
	pbody->listener = this;

	//Sensor
	sensor = Engine::GetInstance().physics.get()->CreateRectangleSensor((int)position.getX(), (int)position.getY() + texH, texW * 12, texH, bodyType::KINEMATIC);
	sensor->ctype = ColliderType::SENSOR;
	sensor->listener = this;

	rangeAttack = Engine::GetInstance().physics.get()->CreateRectangleSensor((int)position.getX() - 32, (int)position.getY() + texH, texW * 2, texH, bodyType::KINEMATIC);
	rangeAttack->ctype = ColliderType::RANGEATTACK;
	rangeAttack->listener = this;

	pathfinding = new Pathfinding();
	ResetPath();

	// Set the gravity of the body
	if (!parameters.attribute("gravity").as_bool()) pbody->body->SetGravityScale(0);

	return true;
}

void Enemy::SetEnemyType(EnemyType et) {
	type = et;
}

bool Enemy::Update(float dt) {

	velocity = b2Vec2(0, -GRAVITY_Y);

	if (currentAnimation == &dmg && currentAnimation->HasFinished()) currentAnimation = &idle;

	if (currentAnimation == &walk) {
		if (directionLeft) {
			velocity.x = -speed;
		}
		else {
			velocity.x = +speed;
		}
	}

	if (followPlayer && !rangePlayer) {
		MovementEnemy(dt);
	}

	if (rangePlayer) {
		currentAnimation = &attack;
	}

	/*
	tempChangeAnimation--;
	if (tempChangeAnimation <= 0) {
		if (currentAnimation == &walk) {
			currentAnimation = &idle;
			tempChangeAnimation = 120;
			de = DirectionEnemy::RIGHT;
		}
		else if (currentAnimation == &idle) {
			currentAnimation = &walk;
			tempChangeAnimation = 20;
			directionLeft = !directionLeft;
			de = DirectionEnemy::LEFT;
		}
	}
	*/

	pbody->body->SetLinearVelocity(velocity);

	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

	if (currentAnimation == &attack)
		Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX() + (flipType == SDL_FLIP_NONE ? - 16 : + 0), (int)position.getY() + 1, flipType, &currentAnimation->GetCurrentFrame());
	else
		Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY() + 1, flipType, &currentAnimation->GetCurrentFrame());

	currentAnimation->Update();

	b2Vec2 enemyPos = pbody->body->GetPosition();
	sensor->body->SetTransform({ enemyPos.x, enemyPos.y }, 0);
	rangeAttack->body->SetTransform({ enemyPos.x, enemyPos.y }, 0);

	if (currentAnimation == &die && currentAnimation->HasFinished()) dead = true;
	return true;
}

bool Enemy::CleanUp()
{
	Engine::GetInstance().textures.get()->UnLoad(texture);
	return true;
}

void Enemy::MovementEnemy(float dt) {
	//Reset
	Vector2D pos = GetPosition();
	Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap(pos.getX(), pos.getY());
	pathfinding->ResetPath(tilePos);

	int max = 20;
	bool found = false;
	while (!found) {
		found = pathfinding->PropagateAStar(MANHATTAN);
		max--;
		if (max == 0) break;
		if (Engine::GetInstance().physics.get()->GetDebug())
			pathfinding->DrawPath();
	}

	if (found) {
		int sizeBread = pathfinding->breadcrumbs.size();
		Vector2D posBread;
		if (sizeBread >= 2) posBread = pathfinding->breadcrumbs[pathfinding->breadcrumbs.size() - 2];
		else posBread = pathfinding->breadcrumbs[pathfinding->breadcrumbs.size() - 1];

		//Movement Enemy
		if (currentAnimation != &die) {
			if (posBread.getX() <= tilePos.getX()) {
				velocity.x = -speed;
				flipType = SDL_FLIP_NONE;
			}
			else {
				velocity.x = speed;
				flipType = SDL_FLIP_HORIZONTAL;
			}
		}
	}
}

void Enemy::SetPosition(Vector2D pos) {
	pos.setX(pos.getX() + texW / 2);
	pos.setY(pos.getY() + texH / 2);
	b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
	pbody->body->SetTransform(bodyPos, 0);
}

Vector2D Enemy::GetPosition() {
	b2Vec2 bodyPos = pbody->body->GetTransform().p;
	Vector2D pos = Vector2D(METERS_TO_PIXELS(bodyPos.x), METERS_TO_PIXELS(bodyPos.y));
	return pos;
}

void Enemy::ResetPath() {
	Vector2D pos = GetPosition();
	Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap(pos.getX(), pos.getY());
	pathfinding->ResetPath(tilePos);
}

void Enemy::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::UNKNOWN:
		break;
	case ColliderType::ATTACKPLAYER:
		if (physA->ctype != ColliderType::SENSOR) {
			currentAnimation = &die;
			currentAnimation->Reset();
		}
		break;
	case ColliderType::PLAYER:
		if (physA->ctype == ColliderType::SENSOR) {
			followPlayer = true;
		}
		if (physA->ctype == ColliderType::RANGEATTACK) {
			rangePlayer = true;
		}

		break;
	default:
		break;
	}
}

void Enemy::OnCollisionEnd(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		if (physA->ctype == ColliderType::SENSOR) {
			followPlayer = false;
		}
		if (physA->ctype == ColliderType::RANGEATTACK) {
			rangePlayer = false;
			currentAnimation->Reset();
			currentAnimation = &idle;
		}
		break;
	default:
		break;
	}
}