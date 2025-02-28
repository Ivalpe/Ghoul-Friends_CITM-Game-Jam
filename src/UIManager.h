#pragma once

#include "Module.h"
#include "Input.h"
#include "Render.h"
#include "vector"
#include "GuiControlButton.h"

class UIManager : public Module {
public:
	UIManager();

	virtual ~UIManager();

	bool Start();

	bool Update(float dt);

	void Add(GuiClass gui, GuiControl* control);

	bool CleanUp();

	void Show(GuiClass gui, bool show);

	int GetSize(GuiClass gui);

private:
	//player lifebar
	bool lifebarFlash = false;
	int flashesCount = 0;
	int flashDuration = 0;
	bool iconFlash = false;
	SDL_Texture* lifebar;

	std::vector<GuiControl*> mainMenu;
	std::vector<GuiControl*> pause;

	SDL_Texture* pauseScreen;
	SDL_Texture* menuScreen;
	SDL_Texture* logo;
	SDL_Texture* deathScreen;
};