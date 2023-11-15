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
	
	// all sprites are 138x88
	idle.PushBack({ 696,1,138,88 });
	idle.PushBack({ 835,1,138,88 });

	longIdle1.PushBack({ 974,1,138,88});
	longIdle1.PushBack({ 1113,1,138,88 });
	longIdle1.PushBack({ 1252,1,138,88 });
	longIdle2.PushBack({ 696,90,138,88 });
	longIdle2.PushBack({ 835,90,138,88 });
	longIdle2.PushBack({ 974,90,138,88 });
	longIdle2.PushBack({ 1113,90,138,88 });
	longIdle2.PushBack({ 1252,90,138,88 });
	idle.speed = 0.05f/16;
	longIdle1.speed = 0.08f/16;
	longIdle1.loop = false;
	longIdle2.speed = 0.07f/16;

	longRun.PushBack({ 1  ,1,138,88 });
	longRun.PushBack({ 140,1,138,88 });
	longRun.PushBack({ 418,1,138,88 });
	longRun.PushBack({ 557,1,138,88 });

	longRun.PushBack({ 1  ,90,138,88 });
	longRun.PushBack({ 140,90,138,88 });
	longRun.PushBack({ 418,90,138,88 });
	longRun.PushBack({ 557,90,138,88 });

	longRun.PushBack({ 1  ,179,138,88 });
	longRun.PushBack({ 140,179,138,88 });
	longRun.speed = 0.16f / 16;
	

	playerRun.PushBack({ 0,240,80,80 });
	playerRun.PushBack({ 80,240,80,80 });
	playerRun.PushBack({ 160,240,80,80 });
	playerRun.PushBack({ 240,240,80,80 });
	playerRun.PushBack({ 320,240,80,80 });


	playerRun.speed = 0.3f/16;
	playerRun.loop = true;

	Jump.PushBack({ 1,1,138,88});
	Jump.PushBack({ 1,268,138,88 });
	Jump.PushBack({ 140,268,138,88 });
	Jump.PushBack({ 279,268,138,88 });
	Jump.speed = 0.2f/16;
	Jump.loop = false;

	epicSpawn.speed = 0.13f / 16;
	epicSpawn.loop = false;
	epicSpawn.PushBack({ 1,548,112,144 });
	epicSpawn.PushBack({ 115,548,112,144 });
	epicSpawn.PushBack({ 229,548,112,144 });
	epicSpawn.PushBack({ 343,548,112,144 });
	epicSpawn.PushBack({ 457,548,112,144 });
	epicSpawn.PushBack({ 571,548,112,144 });
	epicSpawn.PushBack({ 685,548,112,144 });
	epicSpawn.PushBack({ 799,548,112,144 });
	epicSpawn.PushBack({ 913,548,112,144 });
	epicSpawn.PushBack({ 1027,548,112,144 });

	epicSpawn.PushBack({ 1,  839,112,144 });
	epicSpawn.PushBack({ 115,839,112,144 });
	epicSpawn.PushBack({ 229,839,112,144 });
	epicSpawn.PushBack({ 343,839,112,144 });
	epicSpawn.PushBack({ 457,839,112,144 });
	epicSpawn.PushBack({ 571,839,112,144 });
	epicSpawn.PushBack({ 685,839,112,144 });
	epicSpawn.PushBack({ 799,839,112,144 });
	epicSpawn.PushBack({ 913,839,112,144 });
	epicSpawn.PushBack({ 1027,839,112,144 });

	epicSpawn.PushBack({ 1,  984,112,144 });
	epicSpawn.PushBack({ 115,984,112,144 });
	epicSpawn.PushBack({ 229,984,112,144 });
	epicSpawn.PushBack({ 343,984,112,144 });
	epicSpawn.PushBack({ 457,984,112,144 });
	epicSpawn.PushBack({ 571,984,112,144 });
	epicSpawn.PushBack({ 685,984,112,144 });
	epicSpawn.PushBack({ 799,984,112,144 });
	epicSpawn.PushBack({ 913,984,112,144 });
	epicSpawn.PushBack({ 1027,984,112,144 });

	quickSpawn.speed = 0.18f / 16;
	quickSpawn.loop = false;
     /*quickSpawn.PushBack({ 799,548,112,144 });
     quickSpawn.PushBack({ 913,548,112,144 });
     quickSpawn.PushBack({ 1027,548,112,144 });
     quickSpawn.PushBack({ 1,  839,112,144 });*/
     quickSpawn.PushBack({ 115,839,112,144 });
     quickSpawn.PushBack({ 229,839,112,144 });
     quickSpawn.PushBack({ 343,839,112,144 });
     quickSpawn.PushBack({ 457,839,112,144 });
     quickSpawn.PushBack({ 571,839,112,144 });
     quickSpawn.PushBack({ 685,839,112,144 });
     quickSpawn.PushBack({ 799,839,112,144 });
     quickSpawn.PushBack({ 913,839,112,144 });
     quickSpawn.PushBack({ 1027,839,112,144 });
     quickSpawn.PushBack({ 1,  984,112,144 });
     quickSpawn.PushBack({ 115,984,112,144 });
     quickSpawn.PushBack({ 229,984,112,144 });
     quickSpawn.PushBack({ 343,984,112,144 });
     quickSpawn.PushBack({ 457,984,112,144 });
     quickSpawn.PushBack({ 571,984,112,144 });
     quickSpawn.PushBack({ 685,984,112,144 });
     quickSpawn.PushBack({ 799,984,112,144 });
     quickSpawn.PushBack({ 913,984,112,144 });
     quickSpawn.PushBack({ 1027,984,112,144 });

	 groundAttack.PushBack({ 582,242,138,88 });
	 groundAttack.PushBack({ 722,242,138,88 });
	 groundAttack.PushBack({ 862,242,138,88 });
	 groundAttack.PushBack({ 1002,242,138,88, },true);
	 groundAttack.PushBack({ 722,331,138,88 });
	 groundAttack.PushBack({ 722,331,138,88 });
	 groundAttack.PushBack({ 862,331,138,88 }, true);
	 groundAttack.PushBack({ 582,420,138,88 });
	 groundAttack.PushBack({ 722,420,138,88 });
	 groundAttack.PushBack({ 862,420,138,88 });
	 groundAttack.PushBack({ 1002,420,138,88 });
	 groundAttack.PushBack({ 1142,420,138,88 });
	 groundAttack.PushBack({ 1282,420,138,88 });
	
	 
	 groundAttack.loop = false;
	 groundAttack.speed = 0.14f / 16;
	 groundAttack.opportunityFrame = 2;
	 groundAttack.opportunityKey = SDL_SCANCODE_M;

	
	Fall.PushBack({ 418,268,138,88 });
	Fall.PushBack({ 418,357,138,88 });
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
	
	currentAnim = &epicSpawn;
	currentSpawnAnim = &epicSpawn;
	//idleState = true;

	//pickCoinFxId = app->audio->LoadFx("Assets/Audio/Fx/retro-video-game-coin-pickup-38299.ogg");

	return true;
}

bool Player::Update(float dt)
{
	
	if (currentSpawnAnim->HasFinished()) {
		epicSpawn.Reset();
		spawning = false;
		idleState = true;
		quickSpawn.Reset();
		currentSpawnAnim = &quickSpawn;
	}


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


		if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && isJumping == false && isGrounded == true && pbody->body->GetLinearVelocity().y == 0)
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
				currentAnim = &longRun;
			}

			myDir = Direction::LEFT;
			movementx = -speed * dt;
		}

		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {

			IdleTimer.Start();
			if (isGrounded) {
				currentAnim = &longRun;
			}

			myDir = Direction::RIGHT;
			movementx = speed * dt;
		}

		if (app->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN && isGrounded) {

			idleState = false;
			currentAnim = &groundAttack;
			
		}
		if (groundAttack.HasFinished()) {
			groundAttack.Reset();
			idleState = true;
			IdleTimer.Start();
			currentAnim = &idle;
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
		delta_x = pbody->body->GetTransform().p.x - mousex + app->render->camera.x;
		delta_y = pbody->body->GetTransform().p.y - mousey + app->render->camera.y;

		angle_deg = (atan2(delta_y, delta_x) * 180.0000) / 3.1416;

	
	currentAnim->Update();
	spawnFire.Update();
	


	if (myDir == Direction::RIGHT) {
		if(spawning){ app->render->DrawTexture(texture, position.x , position.y - 100, false, &currentAnim->GetCurrentFrame()); }
		else { app->render->DrawTexture(texture, position.x - 16, position.y - 40, false, &currentAnim->GetCurrentFrame()); }
	}
	else {
		if (spawning) { app->render->DrawTexture(texture, position.x, position.y - 100, false, &currentAnim->GetCurrentFrame()); }
		else { app->render->DrawTexture(texture, position.x - 16, position.y - 40, true, &currentAnim->GetCurrentFrame()); }
	}

	if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT && mySpear->isPicked == true) //en vez de w usamos app->input->GetMouseButtonDown(0) == KEY_REPEAT
	{
		app->render->DrawTexture(texture, pbody->body->GetTransform().p.x, pbody->body->GetTransform().p.y - 16, false, 0, 0, angle_deg);
	}
	
	if (app->input->GetKey(SDL_SCANCODE_W) == KEY_UP) 
	{
		if (mySpear->isPicked == true ) {
			mySpear->position = position;
			mySpear->started = false;
			mySpear->daPlatform = false;
			mySpear->isPicked = false;
			mySpear->isSticked = false;
		}
		else if (mySpear->isPicked == false && mySpear->isSticked == true){
			
			b2Vec2 positiondissapera = b2Vec2(-100, -100);
			b2Vec2 positionThePlatform = b2Vec2(mySpear->ThePlatform->body->GetTransform().p.x, mySpear->ThePlatform->body->GetTransform().p.y);
			mySpear->pbody->body->SetTransform(positionThePlatform, 0);
			mySpear->ThePlatform->body->SetTransform(positiondissapera, 0);
			mySpear->daPlatform = true;
			mySpear->isSticked = false;
		}

	}

	if (app->input->GetKey(SDL_SCANCODE_F3) == KEY_UP|| app->input->GetKey(SDL_SCANCODE_F1) == KEY_UP || dead == true && godMode == false) {
		
		Spawn(0);
		dead = false;
	}

	

	if (app->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN && fpsCap == true)
	{
		app->maxFrameDuration = 1000 / 30;
		app->physics->CreateCircle(position.x + 16, position.y + 16, 16, bodyType::DYNAMIC);
		fpsCap = false;
	}
	else if (app->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN && fpsCap == false)
	{
		app->maxFrameDuration = 1000 / 60;
		fpsCap = true;
	}


	/*if (spawnFire.loopCount > 3) {
		spawning = false;
		spawnFire.Reset();
	}*/
	if ((spawning == true)) {

		currentAnim = currentSpawnAnim;
		//app->render->DrawTexture(texture, position.x , position.y - 90, false, &spawnFire.GetCurrentFrame());
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

	if (physA == mySpear->pbody && physB->ctype == ColliderType::PLAYER) {

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
