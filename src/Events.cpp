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
	npcPFPs = Engine::GetInstance().textures.get()->Load("Assets/Textures/npcsBig.png");

	//crow 
	npcPFPsRect.push_back({ 0, 504, 148, 148 });
	//zera
	npcPFPsRect.push_back({ 0, 0, 148, 148 });
	//francesk
	npcPFPsRect.push_back({ 0, 168, 148, 148 });
	//arm guy
	npcPFPsRect.push_back({ 147, 168, 148, 148 });
	//demon
	npcPFPsRect.push_back({ 0, 336, 148, 148 });


	pugi::xml_node levelEvents;
	for (pugi::xml_node levelNode = eventNode.child("event"); levelNode != NULL; levelNode = levelNode.next_sibling("event")) {
		if (levelNode.attribute("level").as_int() == lvl) levelEvents = levelNode;
	}

	
	for (pugi::xml_node npcNode = levelEvents.child("npc"); npcNode != NULL; npcNode = npcNode.next_sibling("npc")) {
		int type = npcNode.attribute("npcType").as_int();

		switch (type) {
		case (int)NPCs::FIRE:
			fire = (NPC*)Engine::GetInstance().entityManager->CreateEntity(EntityType::NPC);
			fire->SetParameters(npcNode);
			fire->SetNPCType((NPCs)type);
			fire->Start();
			break;
		default:
			NPC* n = (NPC*)Engine::GetInstance().entityManager->CreateEntity(EntityType::NPC);
			n->SetParameters(npcNode);
			n->SetNPCType((NPCs)type);
			n->Start();
			npcs.push_back(n);
			break;
		}

		for (pugi::xml_node dialogueNode = npcNode.child("dialogues").child("dialogue"); dialogueNode != NULL; dialogueNode = dialogueNode.next_sibling("dialogue")) {
			Dialogue diag;
			diag.character = dialogueNode.attribute("character").as_int();
			diag.text = dialogueNode.attribute("text").as_string();

			switch (type) {
			case (int)NPCs::ZERA:
				zeraDialogue.push_back(diag);
				break;
			case (int)NPCs::CROW:
				crowDialogue.push_back(diag);
				break;
			case (int)NPCs::ARMGUY:
				armGuyDialogue.push_back(diag);
				break;
			case (int)NPCs::FRANCESK:
				franceskDialogue.push_back(diag);
				break;
			case (int)NPCs::DEMON:
				demonDialogue.push_back(diag);
				break;
			}
		}
	}
}

void Events::CleanUp() {
	if (fire) {
		Engine::GetInstance().physics->DeleteBody((fire)->getBody());
		Engine::GetInstance().entityManager->DestroyEntity(fire);
		fire = nullptr;
	}

	for (auto n : npcs) {
		Engine::GetInstance().physics->DeleteBody((n)->getBody());
		Engine::GetInstance().entityManager->DestroyEntity(n);
	}
	npcs.clear();

	SDL_DestroyTexture(chatbox);
	SDL_DestroyTexture(NyssaPFP);
	SDL_DestroyTexture(npcPFPs);

	crowDialogue.clear();
	zeraDialogue.clear();
	armGuyDialogue.clear();
	franceskDialogue.clear();
	demonDialogue.clear();
	bossDialogue.clear();

	npcPFPsRect.clear();
}

void Events::Update() {
	Engine::GetInstance().scene->DrawText(false);

	if (currentEvent != ActiveEvent::NONE) Engine::GetInstance().scene.get()->eventHappening = true;
	else Engine::GetInstance().scene.get()->eventHappening = false;

	if (Engine::GetInstance().scene.get()->GetGameState() != GameState::PAUSESCREEN) {
		switch (currentEvent) {
		case ActiveEvent::FIRE_EVENT:
			FireEvent();
			break;
		case ActiveEvent::ZERA_EVENT:
			ZeraEvent();
			break;
		case ActiveEvent::CROW_EVENT:
			CrowEvent();
			break;
		case ActiveEvent::ARM_EVENT:
			ArmGuyEvent();
			break;
		case ActiveEvent::FRAN_EVENT:
			FranEvent();
			break;
		case ActiveEvent::DEMON_EVENT:
			DemonEvent();
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

void Events::CrowEvent() {
	Engine::GetInstance().scene->DrawText(false);

	if (timer == 7) {
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_E) == KEY_DOWN) {
			++timer;
			Engine::GetInstance().scene.get()->CreateAttack(EntityType::ATTACKPLAYER, Engine::GetInstance().scene.get()->GetPlayerPosition(), Engine::GetInstance().scene.get()->GetPlayerDirection() == DirectionPlayer::LEFT);
		}
	}
	else {
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
			++timer;
			if (timer >= crowDialogue.size()) {
				Engine::GetInstance().scene->DrawText(false);
				textActive = false;
				currentEvent = ActiveEvent::NONE;
				crowEventDone = true;
				varReset();
			}
		}
	}

	if (textActive) {
		if(timer != 7) Engine::GetInstance().scene->DrawText(true, const_cast<pugi::char_t*>("ContinueDialogue"));
		else Engine::GetInstance().scene->DrawText(true, const_cast<pugi::char_t*>("Arrow"));

		SDL_Rect chatboxRect = { 0, 0, 1920, 1080 };
		SDL_Rect pfpRect = { 0, 0, 178, 178 };
		Engine::GetInstance().render.get()->DrawTexture(chatbox, 0, 0, SDL_FLIP_NONE, &chatboxRect, false, false);

		switch (crowDialogue[timer].character) {
		case -1:
			Engine::GetInstance().render.get()->DrawTexture(NyssaPFP, 486, 836, SDL_FLIP_NONE, &pfpRect, false, false);
			break;
		case (int)NPCs::CROW:
			if(timer != 0) Engine::GetInstance().render.get()->DrawTexture(npcPFPs, 500, 870, SDL_FLIP_NONE, &npcPFPsRect[(int)NPCs::CROW - 1], false, false);
			break;
		}


		Engine::GetInstance().render.get()->DrawText(crowDialogue[timer].text.c_str(), 680, 905, crowDialogue[timer].text.length() * 10, 70);
	}
}

void Events::ZeraEvent() {
	Engine::GetInstance().scene->DrawText(false);

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
		++timer;
		if (timer >= zeraDialogue.size()) {
			Engine::GetInstance().scene->DrawText(false);
			textActive = false;
			currentEvent = ActiveEvent::NONE;
			zeraEventDone = true;
			varReset();
		}
	}

	if (textActive) {
		Engine::GetInstance().scene->DrawText(true, const_cast<pugi::char_t*>("ContinueDialogue"));

		SDL_Rect chatboxRect = { 0, 0, 1920, 1080 };
		SDL_Rect pfpRect = { 0, 0, 178, 178 };
		Engine::GetInstance().render.get()->DrawTexture(chatbox, 0, 0, SDL_FLIP_NONE, &chatboxRect, false, false);

		switch (zeraDialogue[timer].character) {
		case -1:
			Engine::GetInstance().render.get()->DrawTexture(NyssaPFP, 486, 836, SDL_FLIP_NONE, &pfpRect, false, false);
			break;
		case (int)NPCs::ZERA:
			Engine::GetInstance().render.get()->DrawTexture(npcPFPs, 500, 870, SDL_FLIP_NONE, &npcPFPsRect[(int)NPCs::ZERA - 1], false, false);
			break;
		}


		Engine::GetInstance().render.get()->DrawText(zeraDialogue[timer].text.c_str(), 680, 905, zeraDialogue[timer].text.length()*10, 70);
	}
}

void Events::ArmGuyEvent() {
	Engine::GetInstance().scene->DrawText(false);

	if (timer == 0) {
		if (armGuyEventDone) timer = 8;
	}

	if (timer >= 9) {
		if (Engine::GetInstance().scene.get()->regenItem) {
			if (timer == 9) {
				if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F) == KEY_DOWN) {
					++timer;
				}
			}
			else {
				if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
					++timer;
					if (timer >= armGuyDialogue.size()) {
						Engine::GetInstance().scene->DrawText(false);
						textActive = false;
						currentEvent = ActiveEvent::NONE;
						armGuyEventDone = true;
						helpedMan = true;
						varReset();
					}
				}
			}
		}
		else {
			Engine::GetInstance().scene->DrawText(false);
			textActive = false;
			currentEvent = ActiveEvent::NONE;
			armGuyEventDone = true;
			varReset();
		}
	}
	else {
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
			++timer;
			if (timer >= armGuyDialogue.size()) {
				Engine::GetInstance().scene->DrawText(false);
				textActive = false;
				currentEvent = ActiveEvent::NONE;
				armGuyEventDone = true;
				varReset();
			}
		}
	}

	if (textActive) {
		if(timer != 9) Engine::GetInstance().scene->DrawText(true, const_cast<pugi::char_t*>("ContinueDialogue"));
		else Engine::GetInstance().scene->DrawText(true, const_cast<pugi::char_t*>("Heal"));

		SDL_Rect chatboxRect = { 0, 0, 1920, 1080 };
		SDL_Rect pfpRect = { 0, 0, 178, 178 };
		Engine::GetInstance().render.get()->DrawTexture(chatbox, 0, 0, SDL_FLIP_NONE, &chatboxRect, false, false);

		switch (armGuyDialogue[timer].character) {
		case -1:
			Engine::GetInstance().render.get()->DrawTexture(NyssaPFP, 486, 836, SDL_FLIP_NONE, &pfpRect, false, false);
			break;
		case (int)NPCs::ARMGUY:
			Engine::GetInstance().render.get()->DrawTexture(npcPFPs, 500, 870, SDL_FLIP_NONE, &npcPFPsRect[(int)NPCs::ARMGUY - 1], false, false);
			break;
		}


		Engine::GetInstance().render.get()->DrawText(armGuyDialogue[timer].text.c_str(), 680, 905, armGuyDialogue[timer].text.length() * 10, 70);
	}
}

//cave
void Events::FranEvent() {
	Engine::GetInstance().scene->DrawText(false);

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
		++timer;
		if (timer >= franceskDialogue.size()) {
			Engine::GetInstance().scene->DrawText(false);
			textActive = false;
			currentEvent = ActiveEvent::NONE;
			franEventDone = true;
			varReset();
		}
	}

	if (textActive) {
		Engine::GetInstance().scene->DrawText(true, const_cast<pugi::char_t*>("ContinueDialogue"));

		SDL_Rect chatboxRect = { 0, 0, 1920, 1080 };
		SDL_Rect pfpRect = { 0, 0, 178, 178 };
		Engine::GetInstance().render.get()->DrawTexture(chatbox, 0, 0, SDL_FLIP_NONE, &chatboxRect, false, false);

		switch (franceskDialogue[timer].character) {
		case -1:
			Engine::GetInstance().render.get()->DrawTexture(NyssaPFP, 486, 836, SDL_FLIP_NONE, &pfpRect, false, false);
			break;
		case (int)NPCs::FRANCESK:
			Engine::GetInstance().render.get()->DrawTexture(npcPFPs, 500, 870, SDL_FLIP_NONE, &npcPFPsRect[(int)NPCs::FRANCESK - 1], false, false);
			break;
		}


		Engine::GetInstance().render.get()->DrawText(franceskDialogue[timer].text.c_str(), 680, 905, franceskDialogue[timer].text.length() * 10, 70);
	}
}

//mountain
void Events::DemonEvent() {
	Engine::GetInstance().scene->DrawText(false);

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
		++timer;
		if (timer >= demonDialogue.size()) {
			Engine::GetInstance().scene->DrawText(false);
			textActive = false;
			currentEvent = ActiveEvent::NONE;
			demonEventDone = true;
			varReset();
		}
	}

	if (textActive) {
		Engine::GetInstance().scene->DrawText(true, const_cast<pugi::char_t*>("ContinueDialogue"));

		SDL_Rect chatboxRect = { 0, 0, 1920, 1080 };
		SDL_Rect pfpRect = { 0, 0, 178, 178 };
		Engine::GetInstance().render.get()->DrawTexture(chatbox, 0, 0, SDL_FLIP_NONE, &chatboxRect, false, false);

		switch (demonDialogue[timer].character) {
		case -1:
			Engine::GetInstance().render.get()->DrawTexture(NyssaPFP, 486, 836, SDL_FLIP_NONE, &pfpRect, false, false);
			break;
		case (int)NPCs::DEMON:
			Engine::GetInstance().render.get()->DrawTexture(npcPFPs, 500, 870, SDL_FLIP_NONE, &npcPFPsRect[(int)NPCs::DEMON - 1], false, false);
			break;
		}


		Engine::GetInstance().render.get()->DrawText(demonDialogue[timer].text.c_str(), 680, 905, demonDialogue[timer].text.length() * 10, 70);
	}
}