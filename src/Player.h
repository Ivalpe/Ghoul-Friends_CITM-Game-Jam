#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Box2D/Box2D.h"

struct SDL_Texture;

enum class DirectionPlayer {
	LEFT, RIGHT
};

class Player : public Entity
{
public:

	Player();
	
	virtual ~Player();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void SetParameters(pugi::xml_node parameters) {
		this->parameters = parameters;
	}

	// L08 TODO 6: Define OnCollision function for the player. 
	void OnCollision(PhysBody* physA, PhysBody* physB);

	void OnCollisionEnd(PhysBody* physA, PhysBody* physB);

	DirectionPlayer GetDirection() {
		return dp;
	}

	Vector2D GetPosition() {
		return position;
	}

public:

	//Declare player parameters
	float speed = 5.0f;
	SDL_Texture* texture = NULL;
	int texW, texH;

	PhysBody* pbody;
	float jumpForce = .3f; // The force to apply when jumping
	bool isJumping = false; // Flag to check if the player is currently jumping
	DirectionPlayer dp;
	pugi::xml_node parameters;
};