#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Physics.h"
#include "Pathfinding.h"

struct SDL_Texture;

enum class StateChest {
	CLOSED, OPEN
};

class Chest : public Entity
{
public:

	Chest();
	virtual ~Chest();

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

private:

	SDL_Texture* texture;
	const char* texturePath;
	int texW, texH;

	pugi::xml_node parameters;
	Animation* currentAnimation = nullptr;
	Animation open, close;
	StateChest stChest;
	bool openChest = false;
	PhysBody* pbody;
};
