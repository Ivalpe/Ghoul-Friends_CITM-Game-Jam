#include "NPC.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "Map.h"

NPC::NPC() : Entity(EntityType::NPC)
{

}

NPC::~NPC() {
}

bool NPC::Awake() {
	return true;
}

void NPC::SetNPCType(NPCs npcT) {
	type = npcT;
	switch (type) {
	case NPCs::FIRE:
		var.LoadAnimations(parameters.child("animations").child("explosion"));
		break;
	}
}

bool NPC::Start() {

	//Initilize textures
	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();
	position.setX(parameters.attribute("x").as_int());
	position.setY(parameters.attribute("y").as_int());

	//Load animations
	idle.LoadAnimations(parameters.child("animations").child("idle"));
	currentAnimation = &idle;

	if (type == NPCs::CROW) {
		pbody = Engine::GetInstance().physics.get()->CreateCircleSensor((int)position.getX(), (int)position.getY() + texW, texW, bodyType::STATIC);
	}
	else {
		if (texW > 16) pbody = Engine::GetInstance().physics.get()->CreateCircleSensor((int)position.getX(), (int)position.getY() + texW, texW / 4, bodyType::STATIC);
		else pbody = Engine::GetInstance().physics.get()->CreateCircleSensor((int)position.getX(), (int)position.getY() + texW / 2, texW / 2, bodyType::STATIC);
	}

	switch (type) {
	case NPCs::FIRE:
		if (Engine::GetInstance().scene.get()->eventManager->FireEventDone) {
			done = true;
		}
		break;
	case NPCs::ZERA:
		if (!Engine::GetInstance().scene.get()->eventManager->isFireExtinguished) {
			done = true;
			render = false;
		}
		break;
	case NPCs::CROW:
		if (!Engine::GetInstance().scene.get()->eventManager->firstRespawn) {
			done = true;
			render = false;
		}
		break;
	}

	//Assign collider type
	pbody->ctype = ColliderType::NPC;
	pbody->listener = this;

	return true;
}

bool NPC::Update(float dt) {

	switch (type) {
	case NPCs::FIRE:
		if (Engine::GetInstance().scene.get()->eventManager->isFireExtinguished == true and currentAnimation != &var) render = false;

		if (currentAnimation == &var) {
			if (var.HasFinished() and !done) {
				done = true;
				Engine::GetInstance().scene->DrawText(false);
				var.Reset();
				Engine::GetInstance().scene.get()->eventManager->currentEvent = ActiveEvent::FIRE_EVENT;
				if (Engine::GetInstance().scene.get()->eventManager->isFireExtinguished == false) currentAnimation = &idle;
				else render = false;
			}
		}
		break;
	case NPCs::ZERA:
		if (Engine::GetInstance().scene.get()->eventManager->isFireExtinguished) {
			render = true;
			if (!Engine::GetInstance().scene.get()->eventManager->zeraEventDone) done = false;
			else done = false;
		}
		break;
	case NPCs::CROW:
		if (Engine::GetInstance().scene.get()->eventManager->firstRespawn and !Engine::GetInstance().scene.get()->eventManager->crowEventDone) {
			done = false;
			render = true;
		}
		else {
			done = true;
			render = false;
		}
		break;
	}

	if (isActive and Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F) == KEY_DOWN) {
		switch (type) {
		case NPCs::FIRE:
			if (!done) {
				Engine::GetInstance().scene.get()->eventManager->isFireExtinguished = true;
				currentAnimation = &var;
				isActive = false;
			}
			else {
				render = false;
				Engine::GetInstance().scene.get()->eventManager->isFireExtinguished = true;
			}
			break;
		case NPCs::ZERA:
			if (!done) {
				Engine::GetInstance().scene.get()->eventManager->zeraApproached = true;
				isActive = false;
				done = true;
				Engine::GetInstance().scene.get()->eventManager->currentEvent = ActiveEvent::ZERA_EVENT;
			}
			break;
		}
	}

	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

	if (render) {
		if (type == NPCs::CROW) {
			Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX()-4, (int)position.getY() - texW/2, flipType, &currentAnimation->GetCurrentFrame());
		}
		else {
			if (texW > 16) Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY() - texW / 2, flipType, &currentAnimation->GetCurrentFrame());
			else Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY() + 1, flipType, &currentAnimation->GetCurrentFrame());
		}
	}
	currentAnimation->Update();

	return true;
}

bool NPC::CleanUp() {
	return true;
}

void NPC::SetPosition(Vector2D pos) {

	pos.setX(pos.getX() + texW / 2);
	pos.setY(pos.getY() + texH / 2);
	b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
	pbody->body->SetTransform(bodyPos, 0);
}

void NPC::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		isActive = true;
		switch (type) {
		case NPCs::FIRE:
			if(!Engine::GetInstance().scene.get()->eventManager->isFireExtinguished and currentAnimation != &var) Engine::GetInstance().scene->DrawText(true, const_cast<pugi::char_t*>("Fire"));
			break;
		case NPCs::CROW:
			if(!done) Engine::GetInstance().scene.get()->eventManager->currentEvent = ActiveEvent::CROW_EVENT;
			break;
		default:
			if(!done) Engine::GetInstance().scene->DrawText(true, const_cast<pugi::char_t*>("Interaction"));
			break;
		}
		break;
	default:
		break;
	}
}

void NPC::OnCollisionEnd(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		isActive = false;
		switch (type) {
		case NPCs::FIRE:
			if (!done) currentAnimation = &var;
			Engine::GetInstance().scene->DrawText(false);
			break;
		case NPCs::CROW:
			break;
		default:
			Engine::GetInstance().scene->DrawText(false);
			break;
		}
		break;
	default:
		break;
	}
}