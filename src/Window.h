#pragma once

#include "Module.h"
#include "SDL2/SDL_Video.h"
#include "SDL2/SDL.h"

class Window : public Module
{
public:

	Window();

	// Destructor
	virtual ~Window();

	// Called before render is available
	bool Awake();

	// Called before quitting
	bool CleanUp();

	// Changae title
	void SetTitle(const char* title);

	// Retrive window size
	void GetWindowSize(int& width, int& height) const;

	// Retrieve window scale
	int GetScale() const;

public:
	// The window we'll be rendering to
	SDL_Window* window;

	std::string title;
	int width = 1920;
	int height = 1080;
	int scale = 2;
};