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

	respawnPos.setX(position.getX());
	respawnPos.setY(175);

	idle.LoadAnimations(parameters.child("animations").child("idle"));
	currentAnimation = &idle;

	//load animations
	damage.LoadAnimations(parameters.child("animations").child("dmg"));
	death.LoadAnimations(parameters.child("animations").child("die"));
	respawn.LoadAnimations(parameters.child("animations").child("respawn"));
	attack.LoadAnimations(parameters.child("animations").child("shoot"));
	running.LoadAnimations(parameters.child("animations").child("run"));
	jumping.LoadAnimations(parameters.child("animations").child("jump"));
	
	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX(), (int)position.getY(), texW / 2, bodyType::DYNAMIC);

	pbody->listener = this;

	pbody->ctype = ColliderType::PLAYER;
	flipType = SDL_FLIP_NONE;

	return true;
}

bool Player::Update(float dt)
{
	b2Vec2 velocity = b2Vec2(0, -GRAVITY_Y);
	b2Transform pbodyPos;

	switch (Engine::GetInstance().scene.get()->GetGameState())
	{
	case GameState::START:
		isMoving = false;

		//damage reset
		if (isDamaged) {
			if (damage.HasFinished()) {
				damage.Reset();
				isDamaged = false;
			}
		}

		//death and respawn management and reset
		if (life <= 0 and !isDying) {
			isDying = true;
			currentAnimation = &death;
			life = 0;
		}

		if (isDying) {
			if (death.HasFinished() and !hasDied) {
				death.Reset();
				isDying = false;
				hasDied = true;
				life = maxLife;
			}
		} 

		if (startRespawn and hasDied) {
			SetPosition(respawnPos);
			hasDied = false;
			isRespawning = true;
			startRespawn = false;
			currentAnimation = &respawn;
		}

		if (isRespawning) {
			//velocity = b2Vec2(0, jumpForce);
			if (respawn.HasFinished()) {
				respawn.Reset();
				isRespawning = false;
				isDamaged = false;
				currentAnimation = &idle;
			}
		}

		//movement
		if (!isRespawning and !isDying) {
			velocity = b2Vec2(0, pbody->body->GetLinearVelocity().y);

			// Move left
			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
				dp = DirectionPlayer::LEFT;
				velocity.x = -0.2 * 16;
				flipType = SDL_FLIP_NONE;
				isMoving = true;
			}

			// Move right
			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
				dp = DirectionPlayer::RIGHT;
				velocity.x = 0.2 * 16;
				flipType = SDL_FLIP_HORIZONTAL;
				isMoving = true;
			}

			if (coolFire && timer > 0) timer--;

			if (timer == 0) {
				coolFire = false;
			}

			if (!coolFire && Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_E) == KEY_REPEAT) {
				isAttacking = true;
				currentAnimation = &attack;
				timer = fireRate;
			}

			if (isAttacking) {
				if (attack.currentFrame >= (18*attack.speed) and !coolFire) {
					Engine::GetInstance().scene.get()->CreateAttack();
					coolFire = true;
				}
				if (attack.HasFinished()) {
					attack.Reset();
					isAttacking = false;
					currentAnimation = &idle;
				}
			}

			if (isMoving and currentAnimation != &running and !isAttacking) currentAnimation = &running;
			if (!isMoving and !isJumping and !isAttacking) currentAnimation = &idle;
			if (isDamaged and currentAnimation != &damage) isDamaged = false;

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
				if (currentAnimation != &jumping) currentAnimation = &jumping;
			}

			if (regenerationActive) {
				coolHealth--;
				if (coolHealth <= 0) {
					coolHealth = 60 * 5;
					life += (maxLife * 0.01f) * regenerationItems;
				}
			}

			if (life >= maxLife) life = maxLife;
			if (life <= 0) life = 0;
		}


		pbodyPos = pbody->body->GetTransform();
		position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
		position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

		Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY() + 1, flipType, &currentAnimation->GetCurrentFrame());
		currentAnimation->Update();
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
		armor += 0.05;
	}
	else if (item >= 30 && item <= 59) { //Book

	}
	else if (item >= 40 && item <= 79) { //Life
		maxLife += maxLife * (15 / 100.0f);
	}
	else if (item >= 80 && item <= 100) { //Regeneration
		regenerationActive = true;
		regenerationItems++;
	}
}

void Player::SetPosition(Vector2D pos) {
	pos.setX(pos.getX() + texW / 2);
	pos.setY(pos.getY() + texH / 2);
	b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
	pbody->body->SetTransform(bodyPos, 0);
}

void Player::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		//reset the jump flag when touching the ground
		isJumping = false;
		currentAnimation = &idle;
		jumping.Reset();
		break;
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		Engine::GetInstance().physics.get()->DeletePhysBody(physB); // Deletes the body of the item from the physics world
		break;
	case ColliderType::SKELETON:
		if (!isDying and !isRespawning) {
			isDamaged = true;
			damageReceived = physB->damageDone - (physB->damageDone * armor);
			life -= damageReceived;
			currentAnimation = &damage;
		}
		break;
	case ColliderType::RANGE:
		if (!isDying and !isRespawning) {
			isDamaged = true;
			damageReceived = physB->damageDone - (physB->damageDone * armor);;
			life -= damageReceived;
			currentAnimation = &damage;
		}
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