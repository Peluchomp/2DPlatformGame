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
#include "External/SDL2_ttf-2.20.2/include/SDL_ttf.h"

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
	idle.speed = 0.05f/16;
	longIdle1.speed = 0.08f/16;
	longIdle1.loop = false;
	longIdle2.speed = 0.07f/16;

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


	playerRun.speed = 0.1f/16;
	playerRun.loop = true;

	Jump.PushBack({ 0,320,80,100 });
	Jump.PushBack({ 80,320,80,100 });
	Jump.PushBack({ 160,320,80,100 });
	Jump.PushBack({ 240,320,80,100 });
	Jump.speed = 0.2f/16;
	Jump.loop = false;


	
	Fall.PushBack({ 320,320,80,100 });
	Fall.PushBack({ 400,320,80,100 });
	Fall.speed = 0.1f/16;
	Fall.loop = true;

	spawnFire.PushBack({ 0,417,80,130 });
	spawnFire.PushBack({ 80,417,80,130 });
	spawnFire.PushBack({ 160,417,80,130 });
	spawnFire.PushBack({ 240,417,80,130 });
	spawnFire.PushBack({ 320,417,80,130 });
	spawnFire.loop = true;
	spawnFire.speed = 0.2f / 16;

	
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

	int PlayerCoords[8] = {
	34, 12,
	34, 79,
	51, 79,
	51, 12
	};

	

	texture = app->tex->Load(texturePath);

	pbody = app->physics->CreateChain(position.x , position.y ,PlayerCoords,8, bodyType::DYNAMIC);
	pbody->listener = this;
	
	pbody->ctype = ColliderType::PLAYER;

	/*plegs = app->physics->CreateCircle(position.x + 16, position.y + 30, 10, bodyType::STATIC);
	plegs->listener = this;
	plegs->ctype = ColliderType::UNKNOWN;*/
	
	currentAnim = &idle;
	idleState = true;

	//pickCoinFxId = app->audio->LoadFx("Assets/Audio/Fx/retro-video-game-coin-pickup-38299.ogg");

	return true;
}

bool Player::Update(float dt)
{
	if (spawning == false) {
		// Reference to the player's speed
		b2Vec2 Speed = pbody->body->GetLinearVelocity();
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


		jumpDistance += 1 * dt;


		if (Speed.y > 0 && isGrounded == false) /*Falling*/ {
			currentAnim = &Fall;
		}

		if (idleState == true && IdleTimer.ReadSec() < 5 && isGrounded == true) {
			currentAnim = &idle;
		}


		if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
			//
		}
		if (isJumping == true)
		{
			// debe de haber algun problema , aqui le restamos a la velocidad en y pero el dt es mas grande cuanto menor los fps y esto hace que cauga menos/ salte mas
			gravity += 0.05f * dt;

		}


		if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && isJumping == false && isGrounded == true)
		{
			IdleTimer.Start();
			isGrounded = false;
			isJumping = true;
			Jump.Reset();
			currentAnim = &Jump;
			gravity = -17;
		}

		if (gravity >= 0.3f * dt && isJumping == true)
		{
			isJumping = false;
		}


		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {

			IdleTimer.Start();
			if (isGrounded) {
				currentAnim = &playerRun;
			}

			myDir = Direction::LEFT;
			movementx = -speed * dt;
		}

		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {

			IdleTimer.Start();
			if (isGrounded) {
				currentAnim = &playerRun;
			}

			myDir = Direction::RIGHT;
			movementx = speed * dt;
		}

		if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
		{
			if (godMode == true)
			{
				godMode = false;
			}
			else
			{
				godMode = true;
			}

		}
		if (godMode == true)
		{
			gravity = 0;
			if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT) {
				gravity = -10;
			}

			if (app->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) {
				gravity = 10;
			}
		}
	}
		b2Vec2 vel = b2Vec2(movementx, gravity);
		//Set the velocity of the pbody of the player
		pbody->body->SetLinearVelocity(vel);

		//Update player position in pixels
		const float32* x = &pbody->body->GetTransform().p.x;

		position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) + 10;
		position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) + 40;

		SDL_GetMouseState(&mousex, &mousey);
		SDL_Point center{ position.x,position.y };
		SDL_Rect  perim{ position.x,position.y,0,0 };
		delta_x = position.x - mousex;
		delta_y = position.y - mousey;

		angle_deg = (atan2(delta_y, delta_x) * 180.0000) / 3.1416;

	
	currentAnim->Update();
	spawnFire.Update();
	


	if (myDir == Direction::RIGHT) {
		app->render->DrawTexture(texture, position.x - 16, position.y - 40, false, &currentAnim->GetCurrentFrame());
	}
	else {
		app->render->DrawTexture(texture, position.x - 16, position.y - 40, true, &currentAnim->GetCurrentFrame());
	}

	/*if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) 
	{
		app->render->DrawTexture(texture, position.x, position.y - 16, false, 0, 0, angle_deg);
	}
	
	if (app->input->GetKey(SDL_SCANCODE_W) == KEY_UP) 
	{
		mySpear->position = position;
		mySpear->started = false;
	}*/

	if (app->input->GetKey(SDL_SCANCODE_F3) == KEY_UP|| app->input->GetKey(SDL_SCANCODE_F1) == KEY_UP || dead == true && godMode == false) {
		
		Spawn(0);
		dead = false;
	}

	

	if (app->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN && fpsCap == true)
	{
		app->maxFrameDuration = 1000 / 30;
		fpsCap = false;
	}
	else if (app->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN && fpsCap == false)
	{
		app->maxFrameDuration = 1000 / 60;
		fpsCap = true;
	}


	if (spawnFire.loopCount > 3) {
		spawning = false;
		spawnFire.Reset();
	}
	if ((spawning == true)) {

		app->render->DrawTexture(texture, position.x , position.y - 90, false, &spawnFire.GetCurrentFrame());
	}
	
	return true;
}

bool Player::CleanUp()
{

	return true;
}

void Player::OnCollision(PhysBody* physA, PhysBody* physB) {

	if (dead == false) {

		switch (physB->ctype)
		{
		case ColliderType::ITEM:
			LOG("Collision ITEM");
			app->audio->PlayFx(pickCoinFxId);
			break;
		case ColliderType::PLATFORM:
			if (isJumping == true)
				isJumping = false;
			Fall.Reset();
			isGrounded = true;
			LOG("Collision PLATFORM");
			break;
		case ColliderType::SPEAR:

			LOG("Collision SPEAR");
			break;
		case ColliderType::UNKNOWN:
			LOG("Collision UNKNOWN");
			break;

		case ColliderType::INSTAKILL:

			dead = true;
			break;

		}
	}
}


void Player::Spawn(int Level) {
	if (Level == 0) {
		
		spawning = true;
		spawnFire.loopCount = 2;
		float x = position.x = parameters.attribute("x").as_float();
		float y = position.y = parameters.attribute("y").as_float();
		x = PIXEL_TO_METERS(x); y = PIXEL_TO_METERS(y);

		b2Vec2 startPos = { x,y };
		pbody->body->SetTransform(startPos, pbody->body->GetAngle());
	}
}
