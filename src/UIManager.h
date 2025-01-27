#pragma once

#include "Module.h"
#include "Input.h"
#include "Render.h"

class UIManager : public Module {
public:
	UIManager();

	virtual ~UIManager();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

private:
	//player lifebar
	bool lifebarFlash = false;
	int flashesCount = 0;
	int flashDuration = 0;
	bool iconFlash = false;
	SDL_Texture* lifebar;
};