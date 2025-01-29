#include "Boss.h"
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

Boss::Boss() : Entity(EntityType::ENEMY)
{

}

Boss::~Boss() {
}

bool Boss::Awake() {
	return true;
}

bool Boss::Start() {
	followPlayer = false;
	rangePlayer = false;
	speed = 1.9f;

	//Initilize textures
	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();

	//Load animations
	idle.LoadAnimations(parameters.child("animations").child("idle"));
	fly.LoadAnimations(parameters.child("animations").child("fly"));
	summonFire.LoadAnimations(parameters.child("animations").child("summonFire"));
	attackTrident.LoadAnimations(parameters.child("animations").child("attackTrident"));
	dmg.LoadAnimations(parameters.child("animations").child("dmg"));
	die.LoadAnimations(parameters.child("animations").child("die"));
	currentAnimation = &idle;

	//Add a physics to an item - initialize the physics body
	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX(), (int)position.getY() + texW, texW / 2, bodyType::DYNAMIC);

	//Assign collider type
	pbody->ctype = ColliderType::BOSS;
	pbody->listener = this;

	//get life and damage values
	life = parameters.attribute("life").as_int();
	damage = parameters.attribute("damage").as_int();

	//Assign damage that enemy does
	pbody->damageDone = damage - 3;

	//Sensor
	sensorLeft = Engine::GetInstance().physics.get()->CreateRectangleSensor((int)position.getX(), (int)position.getY(), texW * 6, texH * 3, bodyType::KINEMATIC);
	sensorLeft->ctype = ColliderType::RANGELEFT;
	sensorLeft->listener = this;

	//Sensor
	sensorRight = Engine::GetInstance().physics.get()->CreateRectangleSensor((int)position.getX(), (int)position.getY(), texW * 6, texH * 3, bodyType::KINEMATIC);
	sensorRight->ctype = ColliderType::RANGERIGHT;
	sensorRight->listener = this;

	//Sensor
	sensorActive = Engine::GetInstance().physics.get()->CreateRectangleSensor((int)position.getX(), (int)position.getY(), texW * 2, texH * 6, bodyType::KINEMATIC);
	sensorActive->ctype = ColliderType::ACTIVEBOSS;
	sensorActive->listener = this;

	rangeAttack = Engine::GetInstance().physics.get()->CreateRectangleSensor((int)position.getX(), (int)position.getY(), 32, texH, bodyType::KINEMATIC);
	rangeAttack->ctype = ColliderType::RANGE;
	rangeAttack->listener = this;


	// Set the gravity of the body
	if (!parameters.attribute("gravity").as_bool()) pbody->body->SetGravityScale(0);

	return true;
}

void Boss::SetBossType(BossType et) {
	type = et;
}

bool Boss::Update(float dt) {

	if (de == DirectionBoss::LEFT) flipType = SDL_FLIP_NONE;
	else flipType = SDL_FLIP_HORIZONTAL;

	if (currentAnimation == &dmg && currentAnimation->HasFinished()) {
		currentAnimation = &idle;
		dmg.Reset();
	}

	if (life <= 0) {
		currentAnimation = &die;
		isDying = true;
		velocity = { 0,0 };
	}

	if (life <= 0 && currentAnimation->HasFinished()) {
		dead = true;
	}

	if (bossActive && currentAnimation != &dmg && currentAnimation != &die) {
		int ran = rand() % 10 + 1;
		timer++;
		switch (stBoss)
		{
		case StateBoss::IDLE:
			velocity = b2Vec2(0, -GRAVITY_Y);
			if (currentAnimation != &idle) currentAnimation = &idle;
			if (timer == 60) {
				if (lastStBoss == StateBoss::FLYDOWN) {
					lastStBoss = stBoss;
					stBoss = StateBoss::FLYRIGHT;
				}
				else if (lastStBoss == StateBoss::FLYRIGHT) {
					lastStBoss = stBoss;
					stBoss = StateBoss::FLYUP;
				}
				timer = 0;

			}
			break;
		case StateBoss::FLYUP:
			velocity.y = -speed;
			if (currentAnimation != &fly) currentAnimation = &fly;
			if (timer == 40) {
				lastStBoss = stBoss;
				if (ran <= 4) stBoss = StateBoss::FLYIDLE;
				else stBoss = StateBoss::SUMMONFIRE;
				timer = 0;
			}
			break;
		case StateBoss::FLYDOWN:
			velocity.y = +speed;
			if (currentAnimation != &fly) currentAnimation = &fly;
			if (timer == 40) {
				lastStBoss = stBoss;
				if (ran <= 4) stBoss = StateBoss::IDLE;
				else stBoss = StateBoss::TRIDENT;
				timer = 0;
			}
			break;
		case StateBoss::FLYIDLE:
			velocity = b2Vec2(0, 0);
			if (currentAnimation != &fly) currentAnimation = &fly;
			if (timer == 60) {
				if (lastStBoss == StateBoss::FLYUP) {
					stBoss = StateBoss::FLYLEFT;
					lastStBoss = stBoss;
				}
				else if (lastStBoss == StateBoss::FLYLEFT) {
					stBoss = StateBoss::FLYDOWN;
					lastStBoss = stBoss;
				}
				timer = 0;
			}
			break;
		case StateBoss::FLYLEFT:
			velocity = b2Vec2(0, 0);
			velocity.x = -speed * 2;
			if (currentAnimation != &fly) currentAnimation = &fly;
			if (timer == 80) {
				lastStBoss = stBoss;
				if (ran <= 4) stBoss = StateBoss::FLYIDLE;
				else stBoss = StateBoss::SUMMONFIRE;
				timer = 0;
			}
			break;
		case StateBoss::FLYRIGHT:
			velocity = b2Vec2(0, 0);
			velocity.x = +speed * 2;
			if (currentAnimation != &fly) currentAnimation = &fly;
			if (timer == 80) {
				lastStBoss = stBoss;
				if (ran <= 4) stBoss = StateBoss::IDLE;
				else stBoss = StateBoss::TRIDENT;
				timer = 0;
			}
			break;
		case StateBoss::SUMMONFIRE:
			velocity = b2Vec2(0, 0);
			if (currentAnimation != &summonFire) {
				summonFire.Reset();
				currentAnimation = &summonFire;
			}
			if (timer == 60) {
				Engine::GetInstance().scene->CreateAttack(EntityType::FIRESHOOTER, position, lastStBoss == StateBoss::FLYUP);
				if (lastStBoss == StateBoss::FLYUP) {
					stBoss = StateBoss::FLYLEFT;
					lastStBoss = stBoss;
				}
				else if (lastStBoss == StateBoss::FLYLEFT) {
					stBoss = StateBoss::FLYDOWN;
					lastStBoss = stBoss;
				}
				timer = 0;
			}
			break;
		case StateBoss::TRIDENT:
			velocity = b2Vec2(0, -GRAVITY_Y);
			if (currentAnimation != &attackTrident) {
				attackTrident.Reset();
				currentAnimation = &attackTrident;
			}
			if (timer == 55) Engine::GetInstance().scene->CreateAttack(EntityType::BOSSTRIDENT, position, de == DirectionBoss::LEFT);
			if (timer == 60) {
				if (lastStBoss == StateBoss::FLYDOWN) {
					lastStBoss = stBoss;
					stBoss = StateBoss::FLYRIGHT;
				}
				else if (lastStBoss == StateBoss::FLYRIGHT) {
					lastStBoss = stBoss;
					stBoss = StateBoss::FLYUP;
				}
				else  stBoss = StateBoss::FLYUP;
				timer = 0;

			}
			break;
		default:
			break;
		}
	}

	pbody->body->SetLinearVelocity(velocity);

	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

	Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY() + 1, flipType, &currentAnimation->GetCurrentFrame());

	currentAnimation->Update();
	pbody->body->SetLinearVelocity(velocity);

	b2Vec2 enemyPos = pbody->body->GetPosition();
	sensorLeft->body->SetTransform({ enemyPos.x - PIXEL_TO_METERS(16 * 6), PIXEL_TO_METERS(768) }, 0);
	sensorRight->body->SetTransform({ enemyPos.x + PIXEL_TO_METERS(16 * 6), PIXEL_TO_METERS(768) }, 0);
	sensorActive->body->SetTransform({ PIXEL_TO_METERS(3184), PIXEL_TO_METERS(768) }, 0);
	rangeAttack->body->SetTransform({ enemyPos.x, enemyPos.y }, 0);

	if (currentAnimation == &die && currentAnimation->HasFinished()) dead = true;
	return true;
}

bool Boss::CleanUp()
{
	Engine::GetInstance().textures.get()->UnLoad(texture);
	return true;
}

void Boss::SetPosition(Vector2D pos) {
	pos.setX(pos.getX() + texW / 2);
	pos.setY(pos.getY() + texH / 2);
	b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
	pbody->body->SetTransform(bodyPos, 0);
}

Vector2D Boss::GetPosition() {
	b2Vec2 bodyPos = pbody->body->GetTransform().p;
	Vector2D pos = Vector2D(METERS_TO_PIXELS(bodyPos.x), METERS_TO_PIXELS(bodyPos.y));
	return pos;
}

void Boss::OnCollision(PhysBody* physA, PhysBody* physB) {
	int ran;
	switch (physB->ctype)
	{
	case ColliderType::UNKNOWN:
		break;
	case ColliderType::ATTACKPLAYER:
		if (physA->ctype == ColliderType::BOSS) {
			if (life > 0) currentAnimation = &dmg;
			life -= physB->damageDone;
		}
		break;
	case ColliderType::PLAYER:
		if (physA->ctype == ColliderType::ACTIVEBOSS)
			bossActive = true;
		else if (physA->ctype == ColliderType::RANGELEFT)
			de = DirectionBoss::LEFT;
		else if (physA->ctype == ColliderType::RANGERIGHT)
			de = DirectionBoss::RIGHT;
		break;
	default:
		break;
	}
}

void Boss::OnCollisionEnd(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		break;
	case ColliderType::ATTACKPLAYER:
		break;
	default:
		break;
	}
}