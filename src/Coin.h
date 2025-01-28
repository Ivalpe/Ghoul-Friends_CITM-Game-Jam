#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Physics.h"

struct SDL_Texture;

class Coin : public Entity
{
public:

	Coin();
	virtual ~Coin();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

	void OnCollisionEnd(PhysBody* physA, PhysBody* physB);

	void SetPosition(Vector2D pos);

	b2Body* getBody() {
		return pbody->body;
	}

	bool isCollected() {
		return collected;
	}

public:

private:

	SDL_Texture* texture;
	const char* texturePath;
	int texW, texH, item;

	bool collected = false;
	Animation* currentAnimation = nullptr;
	Animation idle;

	PhysBody* pbody;
};