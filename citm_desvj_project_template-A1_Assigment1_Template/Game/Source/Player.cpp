#include "Player.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"

Player::Player() : Entity(EntityType::PLAYER)
{
	name.Create("Player");
}

Player::~Player() {
	
}

bool Player::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();

	return true;
}

bool Player::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);

	pbody = app->physics->CreateCircle(position.x + 16, position.y + 16, 16, bodyType::DYNAMIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::PLAYER;

	plegs = app->physics->CreateCircle(position.x + 16, position.y + 30, 10, bodyType::STATIC);
	plegs->listener = this;
	plegs->ctype = ColliderType::UNKNOWN;

	pickCoinFxId = app->audio->LoadFx("Assets/Audio/Fx/retro-video-game-coin-pickup-38299.ogg");

	return true;
}

bool Player::Update(float dt)
{

	if (isJumping == false)
	gravity = 0.3f * dt;

	movementx = 0;

	
	jumpDistance += 1*dt;

	
	if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
		//
	}
	if (isJumping == true) 
	{
		gravity += 0.5f;
		
	}


	if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && isJumping == false && isGrounded == true) 
	{
		isGrounded = false;
		isJumping = true;
		gravity = -0.75f * 16;
	}
	
	if (gravity >= 0.3f *dt && isJumping == true) 
	{
		isJumping = false;
	}
	

	if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		movementx = -speed * dt;
	}

	if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		movementx = speed * dt;
	}

	b2Vec2 vel = b2Vec2(movementx, gravity);
	//Set the velocity of the pbody of the player
	pbody->body->SetLinearVelocity(vel);

	//Update player position in pixels
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 16;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 16;

	SDL_GetMouseState(&mousex, &mousey);
	SDL_Point center{position.x,position.y};
	SDL_Rect  perim{ position.x,position.y,0,0 };
	delta_x = position.x - mousex;
	delta_y = position.y - mousey;

	angle_deg = (atan2(delta_y, delta_x) * 180.0000) / 3.1416;

	


	app->render->DrawTexture(texture, position.x, position.y);

	if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) 
	{
		app->render->DrawTexture(texture, position.x, position.y - 16, 0, 0, angle_deg);
	}
	
	if (app->input->GetKey(SDL_SCANCODE_W) == KEY_UP) 
	{
		app->entityManager->CreateEntity(EntityType::SPEAR);
	}
	
	return true;
}

bool Player::CleanUp()
{

	return true;
}

void Player::OnCollision(PhysBody* physA, PhysBody* physB) {


	
   	switch (physB->ctype)
	{
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		app->audio->PlayFx(pickCoinFxId);
		break;
	case ColliderType::PLATFORM:
		if (isJumping == true)
			isJumping = false;	
		isGrounded = true;
		LOG("Collision PLATFORM");
		break;
	case ColliderType::SPEAR:
	    
		LOG("Collision SPEAR");
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	}
}

