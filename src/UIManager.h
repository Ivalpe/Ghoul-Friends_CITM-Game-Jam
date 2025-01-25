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
	SDL_Texture* lifebar;
	SDL_Rect barRect = { 0,0,52,8 };
};