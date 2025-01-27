#pragma once

#include "Input.h"
#include "Render.h"
#include "Module.h"

#include "Vector2D.h"

enum class GuiControlType
{
	BUTTON,
	TOGGLE,
	CHECKBOX,
	SLIDER,
	SLIDERBAR,
	COMBOBOX,
	DROPDOWNBOX,
	INPUTBOX,
	VALUEBOX,
	SPINNER
};

enum class GuiClass {
	MAIN_MENU
};

enum class GuiControlState
{
	DISABLED,
	NORMAL,
	FOCUSED,
	PRESSED,
	SELECTED,
};

class GuiControl
{
public:

	// Constructor
	GuiControl(GuiControlType type, int id) : type(type), id(id), state(GuiControlState::NORMAL) {}

	// Constructor
	GuiControl(GuiControlType type, SDL_Rect bounds, const char* text) :
		type(type),
		state(GuiControlState::NORMAL),
		bounds(bounds),
		text(text)
	{
		color.r = 255; color.g = 255; color.b = 255;
	}

	void SetType(GuiClass gc) {
		typeClass = gc;
	}

	GuiClass GetType() {
		return typeClass;
	}

	GuiControlType GetControlType() {
		return type;
	}

	// Called each loop iteration
	virtual bool Update(float dt)
	{
		return true;
	}

	// 
	void SetObserver(Module* module)
	{
		observer = module;
	}

	// 
	void NotifyObserver()
	{
		observer->OnGuiMouseClickEvent(this);
	}

	void Disable() {
		state = GuiControlState::DISABLED;
	}

	void Enable() {
		state = GuiControlState::NORMAL;
	}

	void ShowOn() {
		show = true;
	}

	bool IsShowing() {
		return show;
	}

	void ShowOff() {
		show = false;
	}
public:

	int id;
	GuiControlType type;
	GuiControlState state;

	std::string text;       // Control text (if required)
	SDL_Rect bounds;        // Position and size
	SDL_Color color;        // Tint color

	SDL_Rect section;       // Texture atlas base section

	Module* observer;        // Observer 

	GuiClass typeClass;		//Control what type of button is
	bool show = false;
};