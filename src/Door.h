#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Physics.h"
#include "Pathfinding.h"

struct SDL_Texture;

enum class StateDoor {
	BLOCKED, OPEN
};

class Door : public Entity
{
public:

	Door();
	virtual ~Door();

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

	pugi::xml_node parameters;
	SDL_Texture* texture;
	const char* texturePath;
	int texW, texH;
	Animation* currentAnimation = nullptr;
	Animation door;
	StateDoor stDoor;
	PhysBody* pbody;
};
