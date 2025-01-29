#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Physics.h"
#include "Pathfinding.h"

struct SDL_Texture;

#define ENEMY_SPEED			1

enum class StateBoss {
	IDLE, FLYUP, FLYDOWN, FLYIDLE, FLYLEFT, FLYRIGHT, SUMMONFIRE, TRIDENT
};

enum class DirectionBoss {
	LEFT, RIGHT
};

enum class BossType {
	MOUNTAIN
};


class Boss : public Entity
{
public:

	Boss();
	virtual ~Boss();

	bool Awake();

	bool Start();

	void SetBossType(BossType et);

	BossType GetType() {
		return type;
	}

	bool Update(float dt);

	bool CleanUp();

	void SetParameters(pugi::xml_node parameters) {
		this->parameters = parameters;
	}

	void SetPosition(Vector2D pos);
	Vector2D GetPosition();
	void OnCollision(PhysBody* physA, PhysBody* physB);
	void OnCollisionEnd(PhysBody* physA, PhysBody* physB);

	bool IsDead() {
		return dead;
	}

	b2Body* getBody() {
		return pbody->body;
	}

	b2Body* getLeftSensorBody() {
		return sensorLeft->body;
	}

	b2Body* getRightSensorBody() {
		return sensorRight->body;
	}

	b2Body* getRangeBody() {
		return rangeAttack->body;
	}

	DirectionBoss GetDirection() {
		return de;
	}

private:

	SDL_Texture* texture;
	const char* texturePath;
	int texW, texH;
	bool isDying = false;
	bool isDamaged = false;

	int life;
	int damage;

	BossType type;
	pugi::xml_node parameters;
	Animation* currentAnimation = nullptr;
	Animation idle, fly, summonFire, attackTrident, dmg, die;
	DirectionBoss de;
	SDL_RendererFlip flipType;
	StateBoss stBoss, lastStBoss = StateBoss::FLYRIGHT;

	PhysBody* pbody;
	PhysBody* sensorLeft;
	PhysBody* sensorRight;
	PhysBody* sensorActive;
	bool bossActive;
	PhysBody* rangeAttack;
	bool dead, followPlayer, rangePlayer;
	b2Vec2 velocity;
	float speed;

	bool directionLeft = true;
	int timer = 0;
};
