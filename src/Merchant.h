#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Physics.h"

struct SDL_Texture;

enum class StateMerchant {
	IDLE, SELL
};

enum class DirectionMerchant {
	LEFT, RIGHT
};


class Merchant : public Entity
{
public:

	Merchant();
	virtual ~Merchant();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void SetParameters(pugi::xml_node parameters) {
		this->parameters = parameters;
	}

	void SetPosition(Vector2D pos);
	Vector2D GetPosition();
	void OnCollision(PhysBody* physA, PhysBody* physB);
	void OnCollisionEnd(PhysBody* physA, PhysBody* physB);

	b2Body* getBody() {
		return pbody->body;
	}

	b2Body* getSensorBody() {
		return sensor->body;
	}

private:

	SDL_Texture* texture;
	const char* texturePath;
	int texW, texH;
	bool isDying = false;

	pugi::xml_node parameters;
	Animation* currentAnimation = nullptr;
	Animation idle, sell;
	DirectionMerchant dm;
	SDL_RendererFlip flipType;
	StateMerchant stMerchant;

	PhysBody* pbody;
	PhysBody* sensor;
	b2Vec2 velocity;
	float speed;

	bool directionLeft = true;
};
