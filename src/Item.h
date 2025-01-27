#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Physics.h"

struct SDL_Texture;

class Item : public Entity
{
public:

	Item();
	virtual ~Item();

	bool Awake();

	bool Start();

	void SetTexture(int ran);

	bool Update(float dt);

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

	void OnCollisionEnd(PhysBody* physA, PhysBody* physB);

	void SetPosition(Vector2D pos);

	bool isBuyed() {
		return buyed;
	}

	b2Body* getBody() {
		return pbody->body;
	}

public:

private:

	SDL_Texture* texture;
	const char* texturePath;
	int texW, texH, item;
	bool canBuy = false;
	bool buyed = false;

	Animation* currentAnimation = nullptr;
	Animation idle;

	PhysBody* pbody;
};
