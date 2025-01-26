#include "Player.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "EntityManager.h"

Player::Player() : Entity(EntityType::PLAYER)
{
	name = "Player";
}

Player::~Player() {

}

bool Player::Awake() {
	dp = DirectionPlayer::RIGHT;
	return true;
}

bool Player::Start() {

	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	position.setX(parameters.attribute("ix").as_int());
	position.setY(parameters.attribute("iy").as_int());
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();

	Engine::GetInstance().textures.get()->GetSize(texture, texW, texH);
	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX(), (int)position.getY(), texW / 2, bodyType::DYNAMIC);

	pbody->listener = this;

	pbody->ctype = ColliderType::PLAYER;
	flipType = SDL_FLIP_NONE;

	return true;
}

bool Player::Update(float dt)
{
	b2Vec2 velocity = b2Vec2(0, 0);
	b2Transform pbodyPos;

	//temporary damage reset
	if (isDamaged) {
		++damageCounter;
		if(damageCounter >= 60){
			damageCounter = 0;
			isDamaged = false;
		}
	}

	switch (Engine::GetInstance().scene.get()->GetGameState())
	{
	case GameState::START:
		velocity = b2Vec2(0, pbody->body->GetLinearVelocity().y);

		// Move left
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
			dp = DirectionPlayer::LEFT;
			velocity.x = -0.2 * 16;
			flipType = SDL_FLIP_NONE;
		}

		// Move right
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
			dp = DirectionPlayer::RIGHT;
			velocity.x = 0.2 * 16;
			flipType = SDL_FLIP_HORIZONTAL;
		}

		if (coolFire && timer > 0) timer--;

		if (timer == 0) {
			coolFire = false;
		}

		if (!coolFire && Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_E) == KEY_REPEAT) {
			Engine::GetInstance().scene.get()->CreateAttack();
			coolFire = true;
			timer = fireRate;
		}

		//Jump
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && isJumping == false) {
			// Apply an initial upward force
			pbody->body->ApplyLinearImpulseToCenter(b2Vec2(0, -jumpForce), true);
			isJumping = true;
		}

		// If the player is jumpling, we don't want to apply gravity, we use the current velocity prduced by the jump
		if (isJumping == true)
		{
			velocity.y = pbody->body->GetLinearVelocity().y;
		}


		pbodyPos = pbody->body->GetTransform();
		position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
		position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

		Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY() + 2, flipType);
		break;
	default:
		break;
	}

	pbody->body->SetLinearVelocity(velocity);

	return true;
}

bool Player::CleanUp()
{
	LOG("Cleanup player");
	Engine::GetInstance().textures.get()->UnLoad(texture);
	return true;
}

void Player::AddItem(int item) {
	if (item >= 0 && item <= 19) { //Drums
		fireRate *= 0.8;
	}
	else if (item >= 20 && item <= 39) { //Armor

	}
	else if (item >= 30 && item <= 59) { //Book

	}
	else if (item >= 40 && item <= 79) { //Life

	}
	else if (item >= 50 && item <= 100) { //Regeneration

	}
}

void Player::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		//reset the jump flag when touching the ground
		isJumping = false;
		break;
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		Engine::GetInstance().physics.get()->DeletePhysBody(physB); // Deletes the body of the item from the physics world
		break;
	case ColliderType::SKELETON:
		isDamaged = true;
		damageReceived = physB->damageDone;
		life -= physB->damageDone;
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	default:
		break;
	}
}

void Player::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("End Collision PLATFORM");
		break;
	case ColliderType::ITEM:
		LOG("End Collision ITEM");
		break;
	case ColliderType::UNKNOWN:
		LOG("End Collision UNKNOWN");
		break;
	default:
		break;
	}
}