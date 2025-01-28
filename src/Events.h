#pragma once
#include "Scene.h"
#include "NPC.h"

enum class ActiveEvent {
	NONE, FIRE_EVENT
};

class Events {
public:
	Events();
	virtual ~Events();

	void LoadLevelEvents(int lvl);

	void SetEventParameters(pugi::xml_node node) {
		eventNode = node;
	}

	bool isEventActive() {
		if (currentEvent == ActiveEvent::NONE) return false;
		else return true;
	}

	void Update();

	void varReset() {
		timer = 0;
		subtimer = -1;
		var = 1;
		textActive = true;
	}

	ActiveEvent currentEvent = ActiveEvent::NONE;

	//events
	//lvl 0
	void FireEvent();
	//cave

	//mountain

private:
	pugi::xml_node eventNode;
	SDL_Texture* chatbox;
	SDL_Texture* NyssaPFP;

	int timer = 0;
	int subtimer = -1;
	bool textActive = true;
	int var = 1;
	NPC* fire;

public:
	//bools
	bool isFireExtinguished = false;
};