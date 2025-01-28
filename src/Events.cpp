#include "Events.h"
#include "Scene.h"
#include "Engine.h"
#include "Textures.h"
#include "Render.h"
#include "EntityManager.h"

Events::Events() {
}

Events::~Events() {

}

void Events::LoadLevelEvents(int lvl) {
	chatbox = Engine::GetInstance().textures.get()->Load("Assets/Textures/chatbox.png");
	NyssaPFP = Engine::GetInstance().textures.get()->Load("Assets/Textures/talkingperssinalas.png");

	pugi::xml_node levelEvents;
	for (pugi::xml_node levelNode = eventNode.child("event"); levelNode != NULL; levelNode = levelNode.next_sibling("event")) {
		if (levelNode.attribute("level").as_int() == lvl) levelEvents = levelNode;
	}

	switch (lvl) {
	case (int)LEVELS::LEVEL0:
		for (pugi::xml_node npcNode = levelEvents.child("npc"); npcNode != NULL; npcNode = npcNode.next_sibling("npc")) {
			NPCs type = (NPCs)npcNode.attribute("npc").as_int();
			
			switch (type) {
			case NPCs::FIRE:
				fire = (NPC*)Engine::GetInstance().entityManager->CreateEntity(EntityType::NPC);
				fire->SetParameters(npcNode);
				fire->SetNPCType(type);
				fire->Start();
				break;
			}
		}
		break;
	}
}

void Events::Update() {
	if (currentEvent != ActiveEvent::NONE) Engine::GetInstance().scene.get()->eventHappening = true;
	else Engine::GetInstance().scene.get()->eventHappening = false;

	if (Engine::GetInstance().scene.get()->GetGameState() != GameState::PAUSESCREEN) {
		switch (currentEvent) {
		case ActiveEvent::FIRE_EVENT:
			FireEvent();
			break;
		}
	}
}

//events
//lvl 0
void Events::FireEvent() {
	++timer;
	if (timer <= 120) {
		Engine::GetInstance().render.get()->DrawRectangle({ 0,0,1920, 1089 }, 0, 0, 0, 255, true, false);
	}
	else if (timer <= 840 and currentEvent != ActiveEvent::NONE) {
		subtimer += var;

		if (subtimer == 120) var = -1;
		else if (subtimer == 0) var = 1;

		Engine::GetInstance().render.get()->DrawRectangle({ 0,0,1920, 1089 }, 0, 0, 0, 255 - (1.8*(120-subtimer)), true, false);
	}
	

	if (timer >= 360) {

		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
			Engine::GetInstance().scene->DrawText(false);
			textActive = false;
			currentEvent = ActiveEvent::NONE;
			FireEventDone = true;
			varReset();
		}

		if (textActive) {
			Engine::GetInstance().scene->DrawText(true, const_cast<pugi::char_t*>("ContinueDialogue"));

			SDL_Rect chatboxRect = { 0, 0, 1920, 1080 };
			SDL_Rect pfpRect = { 0, 0, 178, 178 };
			Engine::GetInstance().render.get()->DrawTexture(chatbox, 0, 0, SDL_FLIP_NONE, &chatboxRect, false, false);
			Engine::GetInstance().render.get()->DrawTexture(NyssaPFP, 486, 836, SDL_FLIP_NONE, &pfpRect, false, false);
			Engine::GetInstance().render.get()->DrawText("Nyssa", 700, 846, 200, 90);
			Engine::GetInstance().render.get()->DrawText("¿Que..? ¿Que ha sido eso?", 720, 905, 250, 70);
		}
	}
}