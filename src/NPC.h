#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Physics.h"
#include "Pathfinding.h"

enum class NPCs {
	FIRE, CROW, ZERA, FRANCESK, ARMGUY, DEMON
};

class NPC : public Entity
{
public:

	NPC();
	virtual ~NPC();

	bool Awake();

	bool Start();

	void SetNPCType(NPCs npcT);

	NPCs GetType() {
		return type;
	}

	bool Update(float dt);

	bool CleanUp();

	void SetParameters(pugi::xml_node parameters) {
		this->parameters = parameters;
	}

	b2Body* getBody() {
		return pbody->body;
	}

	void SetPosition(Vector2D pos);

	void OnCollision(PhysBody* physA, PhysBody* physB);
	void OnCollisionEnd(PhysBody* physA, PhysBody* physB);

private:

	bool isActive = false;
	bool render = true;
	bool done = false;

	SDL_Texture* texture;
	const char* texturePath;
	int texW, texH;
	bool isDying = false;
	bool isDamaged = false;

	int life;
	int damage;

	NPCs type;
	pugi::xml_node parameters;
	Animation* currentAnimation = nullptr;
	Animation idle;
	Animation var;
	SDL_RendererFlip flipType;

	PhysBody* pbody;
};