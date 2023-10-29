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

#define IDLE_SECS 5;

Player::Player() : Entity(EntityType::PLAYER)
{
	name.Create("Player");
	
	// idle sprites are 100x80
	idle.PushBack({ 396,0,100,80 });
	idle.PushBack({ 496,0,100,80 });

	longIdle1.PushBack({599,0,100,80});
	longIdle1.PushBack({ 699,0,100,80 });
	longIdle1.PushBack({ 399,80,100,80 });
	longIdle2.PushBack({ 499,80,100,80 });
	longIdle2.PushBack({ 599,80,100,80 });
	longIdle2.PushBack({ 699,80,100,80 });
	longIdle2.PushBack({ 399,160,100,80 });
	longIdle2.PushBack({ 499,160,100,80 });
	idle.speed = 0.05f;
	longIdle1.speed = 0.08f;
	longIdle1.loop = false;
	longIdle2.speed = 0.07f;

	startRun.PushBack({0,0,80,80});
	startRun.PushBack({ 80,0,80,80 });
	startRun.PushBack({ 160,0,80,80 });
	startRun.PushBack({ 240,0,80,80 });
	startRun.PushBack({ 320,0,80,80 });
	
	startRun.PushBack({ 0,80,80,80 });
	startRun.PushBack({ 80,80,80,80 });
	startRun.PushBack({ 160,80,80,80 });
	startRun.PushBack({ 240,80,80,80 });
	startRun.PushBack({ 320,80,80,80 });
	
	startRun.PushBack({ 0,160,80,80 });
	startRun.PushBack({ 80,160,80,80 });
	startRun.PushBack({ 160,160,80,80 });
	startRun.PushBack({ 240,160,80,80 });
	playerRun.PushBack({ 320,160,80,80 });

	playerRun.PushBack({ 0,240,80,80 });
	playerRun.PushBack({ 80,240,80,80 });
	playerRun.PushBack({ 160,240,80,80 });
	playerRun.PushBack({ 240,240,80,80 });
	playerRun.PushBack({ 320,240,80,80 });


	playerRun.speed = 0.1f;
	playerRun.loop = true;


}

Player::~Player() {
	
}

bool Player::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();

	myDir = Direction::RIGHT;

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
	
	currentAnim = &idle;
	idleState = true;

	pickCoinFxId = app->audio->LoadFx("Assets/Audio/Fx/retro-video-game-coin-pickup-38299.ogg");

	return true;
}

bool Player::Update(float dt)
{
	//-----------Idle Animation Logic---------------//
	if (currentAnim == &idle && startIdle == false) {
		startIdle = true;
		idleState = true;
		IdleTimer.Start();
	}
	if (currentAnim == &idle && startIdle == true && IdleTimer.ReadSec() > 5) {
		currentAnim = &longIdle1;

	}
	if (longIdle1.HasFinished()) {
		currentAnim = &longIdle2;
		longIdle1.Reset();
		
	}

	if (isJumping == false)
	gravity = 0.3f * dt;

	movementx = 0;

	
	jumpDistance += 1*dt;



	if (idleState == true && IdleTimer.ReadSec() < 5) { currentAnim = &idle; }

	
	if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
		//
	}
	if (isJumping == true) 
	{
		gravity += 0.5f;
		
	}


	if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && isJumping == false && isGrounded == true) 
	{
		IdleTimer.Start();
		isGrounded = false;
		isJumping = true;
		gravity = -0.75f * 16;
	}
	
	if (gravity >= 0.3f *dt && isJumping == true) 
	{
		isJumping = false;
	}
	

	if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		
		IdleTimer.Start();
		currentAnim = &playerRun;

		myDir = Direction::LEFT;
		movementx = -speed * dt;
	}

	if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		
		IdleTimer.Start();
		currentAnim = &playerRun;

		myDir = Direction::RIGHT;
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

	
	currentAnim->Update();

	if (myDir == Direction::RIGHT) {
		app->render->DrawTexture(texture, position.x - 16, position.y - 40, false, &currentAnim->GetCurrentFrame());
	}
	else {
		app->render->DrawTexture(texture, position.x - 16, position.y - 40, true, &currentAnim->GetCurrentFrame());
	}

	if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) 
	{
		app->render->DrawTexture(texture, position.x, position.y - 16, false, 0, 0, angle_deg);
	}
	
	if (app->input->GetKey(SDL_SCANCODE_W) == KEY_UP) 
	{
		mySpear->position = position;
		mySpear->started = false;
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

