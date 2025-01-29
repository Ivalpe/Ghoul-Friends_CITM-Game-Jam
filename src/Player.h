#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Box2D/Box2D.h"
#include <vector>
#include "Animation.h"

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

	void AddItem(int item);

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

	int GetCoins() {
		return coins;
	}

	void AddCoins(int c) {
		coins += c;
	}

	void SetPosition(Vector2D pos);

public:

	bool canShot = false;

	//Declare player parameters
	float speed = 5.0f;
	SDL_Texture* texture = NULL;
	int texW, texH;

	int life = 100, maxLife = 100;
	int fireRate = 60, timer;
	bool coolFire = false;

	Animation* currentAnimation = nullptr; 
	Animation idle;     
	Animation damage;
	Animation death;
	Animation respawn;
	Animation attack;
	Animation running;
	Animation jumping;

	PhysBody* pbody;
	float jumpForce = .3f; // The force to apply when jumping
	bool isJumping = false; // Flag to check if the player is currently jumping
	bool isMoving = false;
	bool isDamaged = false;
	bool isAttacking = false;
	bool isDying = false;
	bool hasDied = false;
	bool startRespawn = false;
	bool isRespawning = false;
	int damageReceived = 0;
	DirectionPlayer dp;
	pugi::xml_node parameters;
	SDL_RendererFlip flipType;

	Vector2D respawnPos;

	int coolHealth = 60 * 5;
	int regenerationItems = 0;
	bool regenerationActive = false;
	float armor = 0;
	int coins = 0;
};