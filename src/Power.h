#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Physics.h"
#include "Timer.h"

struct SDL_Texture;

enum class StatePower {
	IDLE, DIE, FIRE
};

enum class DirectionPower {
	LEFT, RIGHT
};

enum class TypePower {
	FIREBALL, FIRESHOOTER, ARROW, ATTACKPLAYER
};

class Power : public Entity
{
public:

	Power(EntityType et);
	virtual ~Power();

	bool Awake();

	bool Start(bool inv);

	bool Update(float dt);

	bool CleanUp();

	void SetParameters(pugi::xml_node par, TypePower tp) {
		parameters = par;
		typePower = tp;
	}

	void SetSpeed(b2Vec2 speed) {
		speedFireball = speed;
	}

	void SetPosition(Vector2D pos);

	Vector2D GetPosition();

	void OnCollision(PhysBody* physA, PhysBody* physB);
	bool HasCollision();

	b2Body* getBody() {
		return pbody->body;
	}

private:

	SDL_Texture* texture;
	int texW, texH;
	pugi::xml_node parameters;
	Animation* currentAnimation = nullptr;
	Animation idle, explode, fire;
	DirectionPower df;
	SDL_RendererFlip flipType;
	StatePower statePower;
	PhysBody* pbody;
	bool hasPlayedFire = false;
	TypePower typePower;

	Timer explosionTimer;
	float explosionTime;
	Timer fireTimer;
	float fireTime;

	bool colision;
	bool inverted;
	int damage;

	int coolShoot = 0;
	int maxTime = 120;
	b2Vec2 speed = { 0, 5. };
	b2Vec2 speedFireball;
};
