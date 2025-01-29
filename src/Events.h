#pragma once
#include "Scene.h"
#include "NPC.h"

enum class ActiveEvent {
	NONE, FIRE_EVENT, ZERA_EVENT, CROW_EVENT, ARM_EVENT, FRAN_EVENT, DEMON_EVENT
};

struct Dialogue {
	int character;
	std::string text;
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

	void CleanUp();

	void FullReset();

	ActiveEvent currentEvent = ActiveEvent::NONE;

	//events
	//lvl 0
	void FireEvent();
	void CrowEvent();
	void ZeraEvent();
	void ArmGuyEvent();
	//cave
	void FranEvent();
	//mountain
	void DemonEvent();

private:
	pugi::xml_node eventNode;
	SDL_Texture* chatbox;
	SDL_Texture* NyssaPFP;
	SDL_Texture* npcPFPs;
	std::vector<SDL_Rect> npcPFPsRect;

	int timer = 0;
	int subtimer = -1;
	bool textActive = true;
	int var = 1;

	NPC* fire;
	std::vector<NPC*> npcs;

public:
	//bools
	bool isFireExtinguished = false;
	bool FireEventDone = false;
	bool zeraApproached = false;
	bool zeraEventDone = false;
	bool helpedMan = false;
	bool armGuyEventDone = false;
	bool franEventDone = false;
	bool demonEventDone = false;
	bool bossEventDone = false;
	bool firstRespawn = false;
	bool crowEventDone = false;

	//dialogues
	std::vector<Dialogue> crowDialogue;
	std::vector<Dialogue> zeraDialogue;
	std::vector<Dialogue> armGuyDialogue;
	std::vector<Dialogue> franceskDialogue;
	std::vector<Dialogue> demonDialogue;
	std::vector<Dialogue> bossDialogue;
};