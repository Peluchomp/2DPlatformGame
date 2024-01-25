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
#include "../TitleScreen.h"
#include "Window.h"

#define IDLE_SECS 5;


Player::Player() : Entity(EntityType::PLAYER)
{
	name.Create("player");

}

Player::~Player() {

}

bool SameRectP(SDL_Rect r1, SDL_Rect r2) {
	if (r1.x == r2.x && r1.y == r2.y && r1.w == r2.w && r1.h == r2.h) {
		return true;
	}
	else { return false; }
}

bool Player::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();




	LoadAnimations();

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
	powerMessage.currentAnim = &powerMessage.defaultAnim;
	noSpearIcon.currentAnim = &noSpearIcon.defaultAnim;

	pbody = app->physics->CreateChain(position.x + 35, position.y, PlayerCoords, 8, bodyType::DYNAMIC, ColliderType::PLAYER);
	pbody->listener = this;

	pbody->ctype = ColliderType::PLAYER;

	attackTrigger = app->physics->CreateRectangleSensor(position.x + 110, position.y + 40, 60, 70, bodyType::DYNAMIC, ColliderType::PLAYER);
	attackTrigger->body->SetGravityScale(0);
	attackTrigger->listener = this; 
	attackTrigger->ctype = ColliderType::PLAYER_ATTACK;

	op_attackTrigger = app->physics->CreateRectangleSensor(position.x + 110, position.y + 40, 73, 103, bodyType::DYNAMIC, ColliderType::PLAYER);
	op_attackTrigger->body->SetGravityScale(0);
	op_attackTrigger->listener = this; 
	op_attackTrigger->ctype = ColliderType::PLAYER_ATTACK;

	orbEffect = app->audio->LoadFx(parameters.child("soundEffects").child("orbSound").attribute("audiopath").as_string());
	noSpearEffect = app->audio->LoadFx(parameters.child("soundEffects").child("noSpearSound").attribute("audiopath").as_string());


	const char* hurtEffectPath = parameters.child("hurtEffect").attribute("texturepath").as_string();
	hurtEffectText = app->tex->Load(hurtEffectPath);
	winEffext = app->audio->LoadFx(app->scene->scene_parameter.child("winEffect").attribute("audiopath").as_string());

	currentAnim = &epicSpawn;
	currentSpawnAnim = &epicSpawn;
	mySpear->currentAnim = &mySpear->form1Anim;
	hp = 4;

	SDL_Rect playerRect;

	// Find the minimum and maximum x and y values
	int minX = PlayerCoords[0];
	int maxX = PlayerCoords[0];
	int minY = PlayerCoords[1];
	int maxY = PlayerCoords[1];

	for (int i = 2; i < 8; i += 2) {
		if (PlayerCoords[i] < minX) minX = PlayerCoords[i];
		if (PlayerCoords[i] > maxX) maxX = PlayerCoords[i];
		if (PlayerCoords[i + 1] < minY) minY = PlayerCoords[i + 1];
		if (PlayerCoords[i + 1] > maxY) maxY = PlayerCoords[i + 1];
	}

	// Set SDL_Rect properties based on the calculated values
	playerRect.x = minX;
	playerRect.y = minY;
	playerRect.w = maxX - minX;
	playerRect.h = maxY - minY;

	pbody->collider = playerRect;

	return true;
}

bool Player::Update(float dt)
{
	int X, Y;
	

	

	ManageInvencibility(); /*Check if ifrmaes are still active*/

	if (currentSpawnAnim->HasFinished()) {
		epicSpawn.Reset();
		spawning = false;
		idleState = true;
		quickSpawn.Reset();
		currentSpawnAnim = &quickSpawn;
	}
	if (spawning && app->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN) {
		spawning = false; /*Skip spawn animation*/
	}

	if (hp <= 0) { 

		Spawn(app->scene->currentLvl); 
	}
	if (position.x > 7000 || app->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN) /*Victory condition*/ {
		Spawn(1);
		
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

		if (isJumping == false) {
			gravity = 0.3f * dt;

			if (Attacking && !isGrounded) {
				gravity = 0.1f * dt;
			}
		}
		movementx = 0;


		jumpDistance += 1 * dt;


		if (Speed.y > 0 && isGrounded == false && !Attacking) /*Falling*/ {
			currentAnim = &Fall;
		}

		if (idleState == true && IdleTimer.ReadSec() < 5 && isGrounded == true) {
			currentAnim = &idle;
		}

		InputControls(dt);
		
			
		//----------Activate God mode to phase through enemies-----------------//
		if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
		{
			if (godMode == true)
			{
				b2Fixture* playerFixture = pbody->body->GetFixtureList();

				b2Filter iframePlayerFilter;
				iframePlayerFilter.categoryBits = PLAYER_CATEGORY_BIT;
				iframePlayerFilter.maskBits = GROUND_CATEGORY_BIT | ENEMY_CATEGORY_BIT; // now player will interact with ground and enemies
				iframePlayerFilter.groupIndex = 0;

				playerFixture->SetFilterData(iframePlayerFilter);

				invencibilityCounter = 0;

				godMode = false;
			}
			else
			{
				b2Fixture* playerFixture = pbody->body->GetFixtureList();

				b2Filter iframePlayerFilter;
				iframePlayerFilter.categoryBits = PLAYER_CATEGORY_BIT;
				iframePlayerFilter.maskBits = GROUND_CATEGORY_BIT;   // when invincible, player can only interact with ground    
				iframePlayerFilter.groupIndex = 0;
				playerFixture->SetFilterData(iframePlayerFilter);
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

	if (app->input->GetKey(SDL_SCANCODE_H) == KEY_DOWN) {
	
		evilSpear = (FloorSpears*)app->entityManager->CreateEntity(EntityType::FLOORSPEAR);
		evilSpear->Awake();
		evilSpear->position.y = position.y;
	}

	b2Vec2 vel = b2Vec2(movementx, gravity);
	//Set the velocity of the pbody of the player
	pbody->body->SetLinearVelocity(vel);

	//Update player position in pixels
	const float32* x = &pbody->body->GetTransform().p.x;

	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) + 10;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) + 40;

	SDL_GetMouseState(&mousex, &mousey);
	mousex -= app->render->camera.x;
	mousey -= app->render->camera.y;
	SDL_Point center{ position.x,position.y };
	SDL_Rect  perim{ position.x,position.y,0,0 };
	delta_x = position.x - mousex / 2;
	delta_y = position.y - mousey / 2 - 40;

	//draw position.x/y
	//app->render->DrawCircle(position.x, position.y, 30, 255, 45, 100, 255);
	//app->render->DrawCircle(delta_x, delta_y, 30, 255, 45, 100, 255);

	angle_deg = (atan2(delta_y, delta_x) * 180.0000) / 3.1416;

	if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP && app->titleS->options == false)
	{
		if (mySpear->isPicked == true && mySpear->isThrown == false && mySpear->isSticked == true && mySpear->daPlatform == false && mySpear->platform == false && thrown == true) {
			thrown = false;
		}

		if (mySpear->isPicked == true && thrownCooldown.ReadSec() > 2) {
			if (thrown == false) {
				currentAnim = &spearThrown;
				idleState = false;
				Attacking = true;
				thrown = true;

			}


		}
		else if (mySpear->isPicked == false && mySpear->isSticked == true) {

			b2Vec2 positiondissapera = b2Vec2(-100, -100);
			b2Vec2 positionThePlatform = b2Vec2(mySpear->ThePlatform->body->GetTransform().p.x, mySpear->ThePlatform->body->GetTransform().p.y);
			mySpear->pbody->body->SetTransform(positionThePlatform, 0);
			mySpear->ThePlatform->body->SetTransform(positiondissapera, 0);
			mySpear->daPlatform = true;
			mySpear->isSticked = false;
		}

	}
	else if (mySpear->isPicked == false && position.DistanceTo(mySpear->position) > 400 && mySpear->isSticked == false && mySpear->platform == false)
	{
		b2Vec2 positiondissapera = b2Vec2(-100, -100);



		mySpear->daPlatform = true;
		mySpear->isSticked = false;
	}


	if (app->input->GetKey(SDL_SCANCODE_F3) == KEY_UP || app->input->GetKey(SDL_SCANCODE_F1) == KEY_UP || dead == true && godMode == false) {

		Spawn(app->scene->currentLvl);
		dead = false;
	}


	//Options menu/pause screen toggle
	



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



	if ((spawning == true && powerTransition == false && currentAnim != &poweUpAnim2 && currentAnim != &poweUpAnim3)) {

		if (provisional == b2Vec2{ 999,999 }) {
			provisional = { movementx, gravity };
		}

		movementx = 0; gravity = 0;
		currentAnim = currentSpawnAnim;
		if (currentSpawnAnim->HasFinished()) {
			movementx = provisional.x;
			gravity = provisional.y;
			provisional = b2Vec2{ 999,999 };
		}

	}
	if ((powerTransition == true) && currentAnim != &poweUpAnim2 && currentAnim != &poweUpAnim3) {

		if (provisional == b2Vec2{ 999,999 }) {
			provisional = { movementx, gravity };
		}

		movementx = 0; gravity = 0;
		currentAnim = &poweUpAnim;
		if (poweUpAnim.HasFinished()) {
			poweUpAnim.Reset();

			currentAnim = &poweUpAnim2;
		}
	}
	if (currentAnim == &poweUpAnim2) {
		powerMessage.active = true;

		if (poweUpAnim2.loopCount > 2) {
			poweUpAnim2.Reset();
			currentAnim = &poweUpAnim3;

		}
	}
	if (currentAnim == &poweUpAnim3) {
		if (poweUpAnim3.loopCount > 2) {

			poweUpAnim3.Reset();
			movementx = provisional.x;
			gravity = provisional.y;
			provisional = b2Vec2{ 999,999 };
			powerTransition = false;
			spawning = false;
			powerMessage.active = false;
		}
	}


	if (orbs > 9) {
		orbs = 0;
		myThunder = (Thunder*)app->entityManager->CreateEntity(EntityType::THUNDER);
		myThunder->parameters = app->scene->scene_parameter.child("thunder");


		switch (power) {
		case(PowerLvl::NORMAL):
			power = PowerLvl::MID;
			powerTransition = true;
			spawning = true;
			break;

		case(PowerLvl::MID):
			power = PowerLvl::OP; // not finished
			powerTransition = true;
			spawning = true;
			break;
		}
	}

	if (hurt && knockTimer.ReadMSec() < 200 && knockDir == Direction::RIGHT) {
		pbody->body->SetLinearVelocity(b2Vec2(8, -3));
		currentAnim = &hurtAnim;
	}
	else if (hurt && knockTimer.ReadMSec() < 200 && knockDir == Direction::LEFT) {
		pbody->body->SetLinearVelocity(b2Vec2(-8, -3));
		currentAnim = &hurtAnim;
	}
	else if (knockTimer.ReadMSec() > 200) { hurt = false; }

	// Method that manages the logic of the attack hitbox
	AttackHitBoxManagement();

	//-------------Post update animation blit------------TT
	currentAnim->Update();

	// Blink opacity to signify invencibility frames
	int alpha = 255;
	if(iframes)invencibilityCounter++;
	if (invencibilityCounter > 8) {
		alpha = 110;
		if (invencibilityCounter > 16) {
			invencibilityCounter = 0;
		}
	}
	
	// this variable is for the shadow are, draw every thing with 0,0,0 rgb
	if (app->scene->noir == false) {
		if (myDir == Direction::RIGHT) {

			if (Attacking && power == PowerLvl::OP) { app->render->DrawTexture(texture, position.x - 36 - 30, position.y - 40 - 24, false, &currentAnim->GetCurrentFrame()); }

			else if (powerTransition) { app->render->DrawTexture(texture, position.x - 33, position.y - 45, false, &currentAnim->GetCurrentFrame()); }
			else if (spawning) { app->render->DrawTexture(texture, position.x, position.y - 100, false, &currentAnim->GetCurrentFrame()); }
			else if (!isGrounded) { app->render->DrawTexture(texture, position.x - 50, position.y - 40, false, &currentAnim->GetCurrentFrame(), alpha); }
			else { app->render->DrawTexture(texture, position.x - 36, position.y - 40, false, &currentAnim->GetCurrentFrame(), alpha); }
		}
		else {

			if (Attacking && power == PowerLvl::OP) { app->render->DrawTexture(texture, position.x - 36 - 30, position.y - 40 - 24, true, &currentAnim->GetCurrentFrame()); }
			else if (powerTransition) { app->render->DrawTexture(texture, position.x - 33, position.y - 45, false, &currentAnim->GetCurrentFrame()); }
			else if (spawning) { app->render->DrawTexture(texture, position.x, position.y - 100, false, &currentAnim->GetCurrentFrame()); }
			else if (!isGrounded) { app->render->DrawTexture(texture, position.x - 21, position.y - 40, true, &currentAnim->GetCurrentFrame(), alpha); }
			else { app->render->DrawTexture(texture, position.x - 36, position.y - 40, true, &currentAnim->GetCurrentFrame(), alpha); }
		}
	}
	else {

		if (myDir == Direction::RIGHT) {

			if (Attacking && power == PowerLvl::OP) { app->render->DrawTexture(texture, position.x - 36 - 30, position.y - 40 - 24, false, &currentAnim->GetCurrentFrame()); }

			else if (powerTransition) { app->render->DrawTexture(texture, position.x - 33, position.y - 45, false, &currentAnim->GetCurrentFrame(),255,1, 0,0,0); }
			else if (spawning) { app->render->DrawTexture(texture, position.x, position.y - 100, false, &currentAnim->GetCurrentFrame(),255,1,0,0,0); }
			else if (!isGrounded) { app->render->DrawTexture(texture, position.x - 50, position.y - 40, false, &currentAnim->GetCurrentFrame(), alpha,1,0,0,0); }
			else { app->render->DrawTexture(texture, position.x - 36, position.y - 40, false, &currentAnim->GetCurrentFrame(), alpha,1,0,0,0); }
		}
		else {

			if (Attacking && power == PowerLvl::OP) { app->render->DrawTexture(texture, position.x - 36 - 30, position.y - 40 - 24, true, &currentAnim->GetCurrentFrame(),255,1,0,0,0); }
			else if (powerTransition) { app->render->DrawTexture(texture, position.x - 33, position.y - 45, false, &currentAnim->GetCurrentFrame(),255,1,0,0,0); }
			else if (spawning) { app->render->DrawTexture(texture, position.x, position.y - 100, false, &currentAnim->GetCurrentFrame(),255,1,0,0,0); }
			else if (!isGrounded) { app->render->DrawTexture(texture, position.x - 21, position.y - 40, true, &currentAnim->GetCurrentFrame(), alpha,1,0,0,0); }
			else { app->render->DrawTexture(texture, position.x - 36, position.y - 40, true, &currentAnim->GetCurrentFrame(), alpha,1,0,0,0); }
		}
	}

	if (SpearhasBeenThrown && app->scene->noir == false) {
		app->render->DrawTexture(mySpear->texture, mySpear->position.x, mySpear->position.y, false, &mySpear->currentAnim->GetCurrentFrame());
	}
	else if (SpearhasBeenThrown && app->scene->noir == true) {
		app->render->DrawTexture(mySpear->texture, mySpear->position.x, mySpear->position.y, false, &mySpear->currentAnim->GetCurrentFrame(),alpha,1,0,0,0);
	}

	if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT && mySpear->isPicked == true) //en vez de w usamos app->input->GetMouseButtonDown(0) == KEY_REPEAT
	{
		IdleTimer.Start();
		app->render->DrawTexture(texture, pbody->body->GetTransform().p.x, pbody->body->GetTransform().p.y - 16, false, 0, 0, angle_deg);
	}

	// ----Power up message----------//
	if (powerMessage.active) {
		powerMessage.currentAnim->Update();
		app->render->DrawTexture(texture, position.x - 32, position.y - 85, false, &powerMessage.currentAnim->GetCurrentFrame());
	}
	//----No Spear Icon-----------//

	if (_noSpearIcon && spear_icon_timer.ReadMSec() < 1500) {
		noSpearIcon.currentAnim->Update();
		app->render->DrawTexture(texture, position.x + 28, position.y - 50, false, &noSpearIcon.currentAnim->GetCurrentFrame());
	}
	
	if (hurt && !hurtIcon.defaultAnim.HasFinished()) {
		hurtIcon.currentAnim = &hurtIcon.defaultAnim;
		hurtIcon.currentAnim->Update();
		hurtIcon.defaultAnim.loop = false;
		app->render->DrawTexture(hurtEffectText, position.x -121, position.y -170, false, &hurtIcon.currentAnim->GetCurrentFrame(), 100);
	}
	pbody->collider.x = position.x + pbody->collider.w +8;
	pbody->collider.y = position.y - pbody->collider.h/2 +5;

	if (app->physics->debug) app->render->DrawRectangle(pbody->collider, 200, 020, 200, 255, true);

	app->render->camera.y;

	return true;
}

bool Player::PostUpdate() {

	//----------------Health Bar----------------//
	healthBar = { 25 - (app->render->camera.x / 2), 42 - (app->render->camera.y / 2), hp * 20, 15 };
	app->render->DrawRectangle(SDL_Rect{ 25 - (app->render->camera.x / 2), 42 - (app->render->camera.y / 2), 80,15 }, 78, 0, 0, 255);
	app->render->DrawRectangle(healthBar, 27, 210, 152, 255);

	////----------------Score stuf----------------//
	scoreText = "Score:" + std::to_string(app->scene->score);
	app->render->DrawText(scoreText.c_str(), app->scene->windowW / 2 - 525, app->scene->windowH / 2 - 380, 80, 40);
	// ---------------Orb stuf----------------//
	orbMeter = { 25 - (app->render->camera.x / 2), 20 - (app->render->camera.y / 2), orbs * 10, 15 };
	app->render->DrawRectangle(SDL_Rect{ 25 - (app->render->camera.x / 2), 20 - (app->render->camera.y / 2), 100,15 }, 50, 0, 140, 255);
	app->render->DrawRectangle(orbMeter, 130, 238, 255, 255);

	return true;
}

bool Player::CleanUp()
{

	return true;
}

void Player::OnCollision(PhysBody* physA, PhysBody* physB) {



	if (physA->ctype == ColliderType::PLAYER && dead == false && physB->active) {

		switch (physB->ctype)
		{
		case ColliderType::ENEMY:
			if (physA->ctype == ColliderType::PLAYER) {
				if (godMode == true) return;
				LOG("Collision ENEMY");
				hurtIcon.defaultAnim.Reset();
				hp--;
				hurt = true;
				int _enemyX; int _enemyY;
				physB->GetPosition(_enemyX, _enemyY);
				if (_enemyX + 30 < position.x) {
					knockDir = Direction::RIGHT;
				}
				else {
					knockDir = Direction::LEFT;
				}
				StartIFrames();
			}
			else if (physA->ctype == ColliderType::PLAYER_ATTACK) {
				physA->listener->hp--;
			}
			break;
		case ColliderType::ENEMY_ATTACK:
			LOG("Collision ENEMY ATTACK");
			if (godMode == true) return;
			hurtIcon.defaultAnim.Reset();
			hp--;
			hurt = true;
			int enemyX; int enemyY;
			physB->GetPosition(enemyX, enemyY);
			if (enemyX + 30 < position.x) {
				knockDir = Direction::RIGHT;
			}
			else {
				knockDir = Direction::LEFT;
			}
			StartIFrames();



			break;
		case ColliderType::PLATFORM:
			if (isJumping == true)
				isJumping = false;
			Fall.Reset();
			isGrounded = true;

			if (physB->myEntity != nullptr && physB->myEntity->type == EntityType::CHANDELIER) {

				physB->myEntity->PlayerStandingOnME();
			}
			else if (physA->myEntity != nullptr && physA->myEntity->type == EntityType::CHANDELIER) {

				physA->myEntity->PlayerStandingOnME();
			}

			LOG("Collision PLATFORM");
			break;
		case ColliderType::SPEAR:

			LOG("Collision SPEAR");
			break;
		case ColliderType::BLACK_TRIGGER:
			LOG("Collision UNKNOWN");
			if (physB->myEntity == nullptr && app->scene->noir == false) {
				app->scene->noir = true;
				app->audio->PlayMusic(app->scene->bossMusicPath.GetString());
			}

			break;

		case ColliderType::INSTAKILL:

			dead = true;
			break;
		case ColliderType::ORB:
			LOG("Player touched an orb");
			orbs++;
			app->scene->score += 25;
			app->audio->PlayFx(orbEffect);
			physB->listener->pendingToDestroy = true;
			break;
		case ColliderType::HEALINGORB:
			LOG("Player touched an orb");
			if (hp < 4 && hp > 0)
			hp++;
			app->scene->score += 25;
			app->audio->PlayFx(orbEffect);
			physB->listener->pendingToDestroy = true;
			break;
		case ColliderType::CHECKPOINT:
			b2Vec2 checkpointPos = physB->body->GetPosition();
			
			checkpointX	= METERS_TO_PIXELS( checkpointPos.x);
			checkpointY = METERS_TO_PIXELS( checkpointPos.y);
			
			break;

		}
	}

}

void Player::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
	isTouching += 1;
}

void Player::Spawn(int Level) {

	app->entityManager->ReSpawn();

	if (mySpear->isPicked == false && mySpear->isSticked == true) {

		b2Vec2 positiondissapera = b2Vec2(-100, -100);
		b2Vec2 positionPlayer = b2Vec2(PIXEL_TO_METERS(checkpointX), PIXEL_TO_METERS(checkpointY));
		mySpear->pbody->body->SetTransform(positionPlayer, 0);
		mySpear->ThePlatform->body->SetTransform(positiondissapera, 0);
		mySpear->daPlatform = true;
		mySpear->isSticked = false;
	}

	if (Level == 0) {
		power = PowerLvl::NORMAL;
		hp = 4;
		spawning = true;
		spawnFire.loopCount = 2;
		//para checkear si funciona los checkpoints
	/*	float x = position.x = parameters.attribute("x").as_float();
		float y = position.y = parameters.attribute("y").as_float();
		x = PIXEL_TO_METERS(x); y = PIXEL_TO_METERS(y);*/
		float x = PIXEL_TO_METERS( checkpointX);
		float y =  PIXEL_TO_METERS( checkpointY);
	
		
		b2Vec2 startPos = { x,y };
		pbody->body->SetTransform(startPos, pbody->body->GetAngle());

		pbody->body->SetLinearVelocity(b2Vec2(0, 0));
		pbody->body->SetAngularVelocity(0.0f);
		movementx = 0;

		orbs = 0;
	}
	if (Level == 1) {

		if (app->scene->bossZone) {
			app->scene->bossZone = false;
			app->physics->DestroyObject(app->scene->bossDoor);
		}

		float x;
		float y;
		app->scene->noir = false;
		x = position.x = checkpointX;
		y = position.y = checkpointY;
		if (app->scene->prevLevel == 0) 
		{
			x = position.x = 2 * 40;
			y = position.y = 79 * 40;
			checkpointX = 2 * 40;
			checkpointY = 79 * 40;
		}
	

		app->scene->currentLvl = 1;
		power = PowerLvl::NORMAL;
		hp = 4;
		spawning = true;
		spawnFire.loopCount = 2;
		
		app->render->camera.y = 0;
		x = PIXEL_TO_METERS(x); y = PIXEL_TO_METERS(y);

		b2Vec2 startPos = { x,y };
		pbody->body->SetTransform(startPos, pbody->body->GetAngle());

		pbody->body->SetLinearVelocity(b2Vec2(0, 0));
		pbody->body->SetAngularVelocity(0.0f);
		movementx = 0;

		orbs = 0;

		if (app->scene->prevLevel == 0)/*if it's the first time you enter level0*/ {
			app->audio->PlayFx(winEffext);
			app->physics->breakAll = true;
			
		}
	}

}

// The player attributes are saved and loaded from scene

void Player::LoadAnimations() {

	for (pugi::xml_node node = parameters.child("animations").child("idle").child("frame"); node != NULL; node = node.next_sibling("frame")) {

		idle.PushBack({ node.attribute("x").as_int() , node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() });
		idle.speed = parameters.child("animations").child("idle").child("speed").attribute("value").as_float() / 16;
	}
	for (pugi::xml_node node = parameters.child("animations").child("longIdle1").child("frame"); node != NULL; node = node.next_sibling("frame")) {

		longIdle1.PushBack({ node.attribute("x").as_int() , node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() });
		longIdle1.speed = parameters.child("animations").child("longIdle1").child("speed").attribute("value").as_float() / 16;
		longIdle1.loop = parameters.child("animations").child("longIdle1").child("loop").attribute("value").as_bool();
	}
	for (pugi::xml_node node = parameters.child("animations").child("longIdle2").child("frame"); node != NULL; node = node.next_sibling("frame")) {

		longIdle2.PushBack({ node.attribute("x").as_int() , node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() });
		longIdle2.speed = parameters.child("animations").child("longIdle2").child("speed").attribute("value").as_float() / 16;

	}
	for (pugi::xml_node node = parameters.child("animations").child("longRun").child("frame"); node != NULL; node = node.next_sibling("frame")) {

		longRun.PushBack({ node.attribute("x").as_int() , node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() });
		longRun.speed = parameters.child("animations").child("longRun").child("speed").attribute("value").as_float() / 16;
		//longRun.loop = parameters.child("animations").child("longRun").child("loop").attribute("value").as_bool();
	}
	for (pugi::xml_node node = parameters.child("animations").child("playerRun").child("frame"); node != NULL; node = node.next_sibling("frame")) {

		playerRun.PushBack({ node.attribute("x").as_int() , node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() });
		playerRun.speed = parameters.child("animations").child("playerRun").child("speed").attribute("value").as_float() / 16;
		playerRun.loop = parameters.child("animations").child("playerRun").child("loop").attribute("value").as_bool();
	}
	for (pugi::xml_node node = parameters.child("animations").child("groundAttack").child("frame"); node != NULL; node = node.next_sibling("frame")) {

		groundAttack.PushBack({ node.attribute("x").as_int() , node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() }, node.attribute("opportunity").as_bool(), node.attribute("audio").as_string());
		groundAttack.speed = parameters.child("animations").child("groundAttack").child("speed").attribute("value").as_float() / 16;
		groundAttack.loop = parameters.child("animations").child("groundAttack").child("loop").attribute("value").as_bool();
	}
	for (pugi::xml_node node = parameters.child("animations").child("mid_groundAttack").child("frame"); node != NULL; node = node.next_sibling("frame")) {


		mid_groundAttack.PushBack({ node.attribute("x").as_int() , node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() }, node.attribute("opportunity").as_bool(), node.attribute("audio").as_string());
		mid_groundAttack.speed = parameters.child("animations").child("mid_groundAttack").child("speed").attribute("value").as_float() / 16;
		mid_groundAttack.loop = parameters.child("animations").child(" mid_groundAttack").child("loop").attribute("value").as_bool();
	}
	for (pugi::xml_node node = parameters.child("animations").child("op_groundAttack").child("frame"); node != NULL; node = node.next_sibling("frame")) {


		op_groundAttack.PushBack({ node.attribute("x").as_int() , node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() }, node.attribute("opportunity").as_bool(), node.attribute("audio").as_string());
		op_groundAttack.speed = parameters.child("animations").child("op_groundAttack").child("speed").attribute("value").as_float() / 16;
		op_groundAttack.loop = parameters.child("animations").child(" op_groundAttack").child("loop").attribute("value").as_bool();
	}

	groundAttack.opportunityFrame = 2;
	groundAttack.opportunityKey = SDL_SCANCODE_W;
	groundAttack.opportunityWindow = 0.2f;
	mid_groundAttack.opportunityFrame = 2;
	mid_groundAttack.opportunityKey = SDL_SCANCODE_W;
	mid_groundAttack.opportunityWindow = 0.2f;
	op_groundAttack.opportunityFrame = 2;
	op_groundAttack.opportunityKey = SDL_SCANCODE_W;
	op_groundAttack.opportunityWindow = 0.2f;


	airAttack.loop = false;
	airAttack.speed = 0.19f / 16;
	airAttack.opportunityKey = SDL_SCANCODE_W;
	airAttack.opportunityWindow = 0.15f;
	airAttack.opportunityFrame = 2;
	mid_airAttack.loop = false;
	mid_airAttack.speed = 0.19f / 16;
	mid_airAttack.opportunityKey = SDL_SCANCODE_W;
	mid_airAttack.opportunityWindow = 0.15f;
	mid_airAttack.opportunityFrame = 2;

	op_airAttack.loop = false;
	op_airAttack.speed = 0.19f / 16;
	op_airAttack.opportunityKey = SDL_SCANCODE_W;
	op_airAttack.opportunityWindow = 0.15f;
	op_airAttack.opportunityFrame = 2;

	for (pugi::xml_node node = parameters.child("animations").child("airAttack").child("frame"); node != NULL; node = node.next_sibling("frame")) {

		airAttack.PushBack({ node.attribute("x").as_int() , node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() }, node.attribute("opportunity").as_bool(), node.attribute("audio").as_string());
		airAttack.speed = parameters.child("animations").child("airAttack").child("speed").attribute("value").as_float() / 16;
		airAttack.loop = parameters.child("animations").child("airAttack").child("loop").attribute("value").as_bool();
	}
	for (pugi::xml_node node = parameters.child("animations").child("mid_airAttack").child("frame"); node != NULL; node = node.next_sibling("frame")) {

		mid_airAttack.PushBack({ node.attribute("x").as_int() , node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() }, node.attribute("opportunity").as_bool(), node.attribute("audio").as_string());
		mid_airAttack.speed = parameters.child("animations").child("mid_airAttack").child("speed").attribute("value").as_float() / 16;
		mid_airAttack.loop = parameters.child("animations").child(" mid_airAttack").child("loop").attribute("value").as_bool();
	}
	for (pugi::xml_node node = parameters.child("animations").child("op_airAttack").child("frame"); node != NULL; node = node.next_sibling("frame")) {

		op_airAttack.PushBack({ node.attribute("x").as_int() , node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() }, node.attribute("opportunity").as_bool(), node.attribute("audio").as_string());
		op_airAttack.speed = parameters.child("animations").child("op_airAttack").child("speed").attribute("value").as_float() / 16;
		op_airAttack.loop = parameters.child("animations").child("op_airAttack").child("loop").attribute("value").as_bool();
	}
	for (pugi::xml_node node = parameters.child("animations").child("fall").child("frame"); node != NULL; node = node.next_sibling("frame")) {

		Fall.PushBack({ node.attribute("x").as_int() , node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() });
		Fall.speed = parameters.child("animations").child("fall").child("speed").attribute("value").as_float() / 16;

	}
	for (pugi::xml_node node = parameters.child("animations").child("spawnFire").child("frame"); node != NULL; node = node.next_sibling("frame")) {

		spawnFire.PushBack({ node.attribute("x").as_int() , node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() }, false, node.attribute("audio").as_string());
		spawnFire.speed = parameters.child("animations").child("spawnFire").child("speed").attribute("value").as_float() / 16;
	}
	for (pugi::xml_node node = parameters.child("animations").child("spearThrown").child("frame"); node != NULL; node = node.next_sibling("frame")) {

		spearThrown.PushBack({ node.attribute("x").as_int() , node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() }, false, node.attribute("audio").as_string());
		spearThrown.speed = parameters.child("animations").child("spearThrown").child("speed").attribute("value").as_float() / 16;
		spearThrown.loop = parameters.child("animations").child("spearThrown").child("loop").attribute("value").as_bool();
	}
	for (pugi::xml_node node = parameters.child("animations").child("jump").child("frame"); node != NULL; node = node.next_sibling("frame")) {

		Jump.PushBack({ node.attribute("x").as_int() , node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() }, false, node.attribute("audio").as_string());
		Jump.speed = parameters.child("animations").child("jump").child("speed").attribute("value").as_float() / 16;
		Jump.loop = parameters.child("animations").child("jump").child("loop").attribute("value").as_bool();
	}

	for (pugi::xml_node node = parameters.child("animations").child("epicSpawn").child("frame"); node != NULL; node = node.next_sibling("frame")) {

		epicSpawn.PushBack({ node.attribute("x").as_int() , node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() }, false, node.attribute("audio").as_string());
		epicSpawn.speed = parameters.child("animations").child("epicSpawn").child("speed").attribute("value").as_float() / 16;
		epicSpawn.loop = parameters.child("animations").child("epicSpawn").child("loop").attribute("value").as_bool();
	}
	//epicSpawn.speed = 0.13f/16;


	for (pugi::xml_node node = parameters.child("animations").child("quickSpawn").child("frame"); node != NULL; node = node.next_sibling("frame")) {

		quickSpawn.PushBack({ node.attribute("x").as_int() , node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() }, false, node.attribute("audio").as_string());
		quickSpawn.speed = parameters.child("animations").child("quickSpawn").child("speed").attribute("value").as_float() / 16;
		quickSpawn.loop = parameters.child("animations").child("quickSpawn").child("loop").attribute("value").as_bool();
	}
	for (pugi::xml_node node = parameters.child("animations").child("powerUp").child("frame"); node != NULL; node = node.next_sibling("frame")) {

		poweUpAnim.PushBack({ node.attribute("x").as_int() , node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() });
		poweUpAnim.speed = parameters.child("animations").child("powerUp").child("speed").attribute("value").as_float() / 16;
		poweUpAnim.loop = parameters.child("animations").child("powerUp").child("loop").attribute("value").as_bool();
	}
	for (pugi::xml_node node = parameters.child("animations").child("powerUp2").child("frame"); node != NULL; node = node.next_sibling("frame")) {

		poweUpAnim2.PushBack({ node.attribute("x").as_int() , node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() });
		poweUpAnim2.speed = parameters.child("animations").child("powerUp2").child("speed").attribute("value").as_float() / 16;
		poweUpAnim2.loop = parameters.child("animations").child("powerUp2").child("loop").attribute("value").as_bool();
	}
	for (pugi::xml_node node = parameters.child("animations").child("powerUp3").child("frame"); node != NULL; node = node.next_sibling("frame")) {

		poweUpAnim3.PushBack({ node.attribute("x").as_int() , node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() });
		poweUpAnim3.speed = parameters.child("animations").child("powerUp3").child("speed").attribute("value").as_float() / 16;
		poweUpAnim3.loop = parameters.child("animations").child("powerUp3").child("loop").attribute("value").as_bool();
	}
	//----------------------Icons----------------------//
	for (pugi::xml_node node = parameters.child("popUp").child("frame"); node != NULL; node = node.next_sibling("frame")) {

		powerMessage.defaultAnim.PushBack({ node.attribute("x").as_int() , node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() });
		powerMessage.defaultAnim.speed = parameters.child("popUp").child("speed").attribute("value").as_float() / 16;
		powerMessage.defaultAnim.loop = parameters.child("animations").child("popUp").child("loop").attribute("value").as_bool();
	}
	for (pugi::xml_node node = parameters.child("NoSpear").child("frame"); node != NULL; node = node.next_sibling("frame")) {

		noSpearIcon.defaultAnim.PushBack({ node.attribute("x").as_int() , node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() });
		noSpearIcon.defaultAnim.speed = parameters.child("NoSpear").child("speed").attribute("value").as_float() / 16;
		noSpearIcon.defaultAnim.loop = parameters.child("NoSpear").child("loop").attribute("value").as_bool();
	}
	for (pugi::xml_node node = parameters.child("animations").child("hurt").child("frame"); node != NULL; node = node.next_sibling("frame")) {

		hurtAnim.PushBack({ node.attribute("x").as_int() , node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() });
		hurtAnim.speed = parameters.child("animations").child("hurt").child("speed").attribute("value").as_float() / 16;
		hurtAnim.loop = parameters.child("animations").child("hurt").child("loop").attribute("value").as_bool();
	}

	for (pugi::xml_node node = parameters.child("hurtEffect").child("frame"); node != NULL; node = node.next_sibling("frame")) {

		hurtIcon.defaultAnim.PushBack({ node.attribute("x").as_int() , node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() }, false, node.attribute("audio").as_string());
		hurtIcon.defaultAnim.speed = parameters.child("hurtEffect").child("speed").attribute("value").as_float() / 16;
		hurtIcon.defaultAnim.loop = parameters.child("animations").child("speed").child("loop").attribute("value").as_bool();
	}
}


// This function changes the offsets of the attack sensor depending on animation, stage of the animation or current power lvl
void Player::AttackHitBoxManagement() {


	if (Attacking) {
		if (power != PowerLvl::OP) {
			attackTrigger->active = true; op_attackTrigger->active = false;
		}
		else { attackTrigger->active = false; op_attackTrigger->active = true; }
	}
	else {
		attackTrigger->active = false;
		op_attackTrigger->active = false;
	}
	if (isGrounded == true) {
		switch (power) {

		case PowerLvl::NORMAL:
			if (myDir == Direction::RIGHT) {
				attackTrigger->body->SetTransform(pbody->body->GetPosition() + b2Vec2(1.5f, 0.8f), 0.0f);

				// frames where the collider has to be behind
				SDL_Rect r{ 722 ,331 ,138 ,88 };
				SDL_Rect r2{ 862 ,331 ,138 ,88 };
				SDL_Rect r3{ 582 ,420 ,138 ,88 };

				/*------Second attack------*/
				if (currentAnim == &groundAttack && ((groundAttack.GetCurrentFrame().x == r.x && groundAttack.GetCurrentFrame().y == r.y) || (groundAttack.GetCurrentFrame().x == r2.x && groundAttack.GetCurrentFrame().y == r2.y) || (groundAttack.GetCurrentFrame().x == r3.x && groundAttack.GetCurrentFrame().y == r3.y))) {

					LOG("Move hitboxes");
					attackTrigger->body->SetTransform(pbody->body->GetPosition() + b2Vec2(0.2f, 0.8f), 0.0f);
				}/*First attck*/


				// frames where the collider has to be deActive
				SDL_Rect deavtice1{ 722 ,420 ,138 ,88 };
				SDL_Rect deavtice2{ 862 ,420 ,138 ,88 };
				SDL_Rect deavtice3{ 1002 ,420 ,138 ,88 };

				if (currentAnim == &groundAttack && ((groundAttack.GetCurrentFrame().x == deavtice1.x && groundAttack.GetCurrentFrame().y == deavtice1.y) || (groundAttack.GetCurrentFrame().x == deavtice2.x && groundAttack.GetCurrentFrame().y == deavtice2.y) || (groundAttack.GetCurrentFrame().x == deavtice3.x && groundAttack.GetCurrentFrame().y == deavtice3.y))) {

					LOG("Move hitboxes");
					attackTrigger->active = false;
				}
			}
			else {

				attackTrigger->body->SetTransform(pbody->body->GetPosition() + b2Vec2(1.5 - 1.2f, 0.8f), 0.0f);

				// frames where the collider has to be behind
				SDL_Rect r{ 722 ,331 ,138 ,88 };
				SDL_Rect r2{ 862 ,331 ,138 ,88 };
				SDL_Rect r3{ 582 ,420 ,138 ,88 };

				if (currentAnim == &groundAttack && ((groundAttack.GetCurrentFrame().x == r.x && groundAttack.GetCurrentFrame().y == r.y) || (groundAttack.GetCurrentFrame().x == r2.x && groundAttack.GetCurrentFrame().y == r2.y) || (groundAttack.GetCurrentFrame().x == r3.x && groundAttack.GetCurrentFrame().y == r3.y))) {

					LOG("Move hitboxes");
					attackTrigger->body->SetTransform(pbody->body->GetPosition() + b2Vec2(0.2f + 1.3f, 0.8f), 0.0f);
				}

				// frames where the collider has to be deActive
				SDL_Rect deavtice1{ 722 ,420 ,138 ,88 };
				SDL_Rect deavtice2{ 862 ,420 ,138 ,88 };
				SDL_Rect deavtice3{ 1002 ,420 ,138 ,88 };

				if (currentAnim == &groundAttack && ((groundAttack.GetCurrentFrame().x == deavtice1.x && groundAttack.GetCurrentFrame().y == deavtice1.y) || (groundAttack.GetCurrentFrame().x == deavtice2.x && groundAttack.GetCurrentFrame().y == deavtice2.y) || (groundAttack.GetCurrentFrame().x == deavtice3.x && groundAttack.GetCurrentFrame().y == deavtice3.y))) {

					LOG("Move hitboxes");
					attackTrigger->active = false;
				}

			}
			break;

		case PowerLvl::MID:
			if (myDir == Direction::RIGHT) {
				attackTrigger->body->SetTransform(pbody->body->GetPosition() + b2Vec2(1.5f, 0.8f), 0.0f);

				// frames where the collider has to be behind
				SDL_Rect r{ 1562 ,331 ,138 ,88 };
				SDL_Rect r2{ 1702 ,331 ,138 ,88 };
				SDL_Rect r3{ 1842 ,331 ,138 ,88 };

				//-----------second attack------------//
				if (currentAnim == &mid_groundAttack && ((mid_groundAttack.GetCurrentFrame().x == r.x && mid_groundAttack.GetCurrentFrame().y == r.y) || (mid_groundAttack.GetCurrentFrame().x == r2.x && mid_groundAttack.GetCurrentFrame().y == r2.y) || (mid_groundAttack.GetCurrentFrame().x == r3.x && mid_groundAttack.GetCurrentFrame().y == r3.y))) {



					LOG("Move hitboxes");
					attackTrigger->body->SetTransform(pbody->body->GetPosition() + b2Vec2(0.2f, 0.8f), 0.0f);
				} /*First attck*/

				// frames where the collider has to be deActive
				SDL_Rect deavtice1{ 1702 ,420 ,138 ,88 };
				SDL_Rect deavtice2{ 1842 ,420 ,138 ,88 };
				SDL_Rect deavtice3{ 1562 ,420 ,138 ,88 };

				//-----------third attack------------//
				if (currentAnim == &mid_groundAttack && ((mid_groundAttack.GetCurrentFrame().x == deavtice1.x && mid_groundAttack.GetCurrentFrame().y == deavtice1.y) || (mid_groundAttack.GetCurrentFrame().x == deavtice2.x && mid_groundAttack.GetCurrentFrame().y == deavtice2.y) || (mid_groundAttack.GetCurrentFrame().x == deavtice3.x && mid_groundAttack.GetCurrentFrame().y == deavtice3.y))) {
					LOG("Move hitboxes");
					attackTrigger->active = false;
				}
			}
			else {

				attackTrigger->body->SetTransform(pbody->body->GetPosition() + b2Vec2(1.5 - 1.2f, 0.8f), 0.0f);

				// frames where the collider has to be behind
				SDL_Rect r{ 1562 ,331 ,138 ,88 };
				SDL_Rect r2{ 1702 ,331 ,138 ,88 };
				SDL_Rect r3{ 1842 ,331 ,138 ,88 };

				//-----------second attack------------//
				if (currentAnim == &mid_groundAttack && ((mid_groundAttack.GetCurrentFrame().x == r.x && mid_groundAttack.GetCurrentFrame().y == r.y) || (mid_groundAttack.GetCurrentFrame().x == r2.x && mid_groundAttack.GetCurrentFrame().y == r2.y) || (mid_groundAttack.GetCurrentFrame().x == r3.x && mid_groundAttack.GetCurrentFrame().y == r3.y))) {
					LOG("Move hitboxes");
					attackTrigger->body->SetTransform(pbody->body->GetPosition() + b2Vec2(0.2f + 1.3f, 0.8f), 0.0f);
				}

				// frames where the collider has to be deActive
				SDL_Rect deavtice1{ 1702 ,420 ,138 ,88 };
				SDL_Rect deavtice2{ 1842 ,420 ,138 ,88 };
				SDL_Rect deavtice3{ 1562 ,420 ,138 ,88 };

				//-----------third attack------------//
				if (currentAnim == &mid_groundAttack && ((mid_groundAttack.GetCurrentFrame().x == deavtice1.x && mid_groundAttack.GetCurrentFrame().y == deavtice1.y) || (mid_groundAttack.GetCurrentFrame().x == deavtice2.x && mid_groundAttack.GetCurrentFrame().y == deavtice2.y) || (mid_groundAttack.GetCurrentFrame().x == deavtice3.x && mid_groundAttack.GetCurrentFrame().y == deavtice3.y))) {
					LOG("Move hitboxes");
					attackTrigger->active = false;
				}

			}
			break;
		case PowerLvl::OP:
			if (myDir == Direction::RIGHT) {
				op_attackTrigger->body->SetTransform(pbody->body->GetPosition() + b2Vec2(1.5f, 0.8f), 0.0f);

				// frames where the collider has to be behind
				SDL_Rect r{ 2191 ,1 ,   198  ,133 };
				SDL_Rect r2{ 1991 ,135 ,198 ,133 };
				SDL_Rect r3{ 2191 ,135 ,198 ,133 };

				//-----------second attack------------//
				if (currentAnim == &op_groundAttack && ((op_groundAttack.GetCurrentFrame().x == r.x && op_groundAttack.GetCurrentFrame().y == r.y) || (op_groundAttack.GetCurrentFrame().x == r2.x && op_groundAttack.GetCurrentFrame().y == r2.y) || (op_groundAttack.GetCurrentFrame().x == r3.x && op_groundAttack.GetCurrentFrame().y == r3.y))) {



					LOG("Move hitboxes");
					op_attackTrigger->body->SetTransform(pbody->body->GetPosition() + b2Vec2(-0.2f, 0.8f), 0.0f);
				} /*First attck*/

				// frames where the collider has to be deActive
				SDL_Rect deavtice1{ 1991 ,269 ,198 ,133 };
				SDL_Rect deavtice2{ 2191 ,269,  198 ,133 };
				SDL_Rect deavtice3{ 2391 ,1 ,198 ,133 };
				SDL_Rect deavtice4{ 2391,135,198,133 };

				//-----------third attack------------//
				if (currentAnim == &op_groundAttack && (SameRectP(deavtice1, currentAnim->GetCurrentFrame()) || SameRectP(deavtice2, currentAnim->GetCurrentFrame()) || SameRectP(deavtice3, currentAnim->GetCurrentFrame()) || SameRectP(deavtice4, currentAnim->GetCurrentFrame()))) {
					LOG("Move hitboxes");
					op_attackTrigger->active = false;

				}

				// elongated stab
				SDL_Rect extended1 = { 2391 ,269 ,198 ,133 };
				SDL_Rect extended2 = { 2391, 403 ,198 ,133 };
				if (SameRectP(currentAnim->GetCurrentFrame(), extended1) || SameRectP(currentAnim->GetCurrentFrame(), extended2)) {
					op_attackTrigger->body->SetTransform(pbody->body->GetPosition() + b2Vec2(0.6f + 1.3f, 0.8f), 0.0f);
				}

			}
			else {

				op_attackTrigger->body->SetTransform(pbody->body->GetPosition() + b2Vec2(1.5 - 1.2f, 0.8f), 0.0f);

				// frames where the collider has to be behind
				SDL_Rect r{ 2191 ,1 ,   198  ,133 };
				SDL_Rect r2{ 1991 ,135 ,198 ,133 };
				SDL_Rect r3{ 2191 ,135 ,198 ,133 };

				//-----------second attack------------//
				if (currentAnim == &op_groundAttack && ((op_groundAttack.GetCurrentFrame().x == r.x && op_groundAttack.GetCurrentFrame().y == r.y) || (op_groundAttack.GetCurrentFrame().x == r2.x && op_groundAttack.GetCurrentFrame().y == r2.y) || (op_groundAttack.GetCurrentFrame().x == r3.x && mid_groundAttack.GetCurrentFrame().y == r3.y))) {
					LOG("Move hitboxes");
					op_attackTrigger->body->SetTransform(pbody->body->GetPosition() + b2Vec2(0.2f + 1.3f, 0.8f), 0.0f);
				}

				// frames where the collider has to be deActive
				SDL_Rect deavtice1{ 1991 ,269 ,198 ,133 };
				SDL_Rect deavtice2{ 2191 ,269,  198 ,133 };
				SDL_Rect deavtice3{ 2391 ,1 ,198 ,133 };
				SDL_Rect deavtice4{ 2391,135,198,133 };

				//-----------third attack------------//
				if (currentAnim == &op_groundAttack && (SameRectP(deavtice1, currentAnim->GetCurrentFrame()) || SameRectP(deavtice2, currentAnim->GetCurrentFrame()) || SameRectP(deavtice3, currentAnim->GetCurrentFrame()) || SameRectP(deavtice4, currentAnim->GetCurrentFrame()))) {
					LOG("Move hitboxes");
					op_attackTrigger->active = false;

				}

				SDL_Rect extended1 = { 2391 ,269 ,198 ,133 };
				SDL_Rect extended2 = { 2391, 403 ,198 ,133 };
				if (SameRectP(currentAnim->GetCurrentFrame(), extended1) || SameRectP(currentAnim->GetCurrentFrame(), extended2)) {
					op_attackTrigger->body->SetTransform(pbody->body->GetPosition() + b2Vec2(-0.2f, 0.8f), 0.0f);
				}
			}
			break;
		}
	}
	else if (isGrounded == false) {
		switch (power) {

		case PowerLvl::NORMAL:
			if (myDir == Direction::RIGHT) {
				attackTrigger->body->SetTransform(pbody->body->GetPosition() + b2Vec2(1.5f, 0.8f), 0.0f);
				SDL_Rect deavtice1{ 1282 ,598 ,138 ,88 };
				SDL_Rect deavtice2{ 1422 ,598 ,138 ,88 };
				if (currentAnim == &airAttack && (SameRectP(airAttack.GetCurrentFrame(), deavtice1) || SameRectP(airAttack.GetCurrentFrame(), deavtice2))) {
					attackTrigger->active = false;
				}
			}
			else if (myDir == Direction::LEFT) {
				attackTrigger->body->SetTransform(pbody->body->GetPosition() + b2Vec2(1.5 - 1.2f, 0.8f), 0.0f);
				SDL_Rect deavtice1{ 1282 ,598 ,138 ,88 };
				SDL_Rect deavtice2{ 1422 ,598 ,138 ,88 };
				if (currentAnim == &airAttack && (SameRectP(airAttack.GetCurrentFrame(), deavtice1) || SameRectP(airAttack.GetCurrentFrame(), deavtice2))) {
					attackTrigger->active = false;
				}

			}
			break;

		case PowerLvl::MID:
			if (myDir == Direction::RIGHT) {
				attackTrigger->body->SetTransform(pbody->body->GetPosition() + b2Vec2(1.5f, 0.8f), 0.0f);
				SDL_Rect deavtice1{ 1702 ,598 ,138 ,88 };
				SDL_Rect deavtice2{ 1842 ,598 ,138 ,88 };
				if (currentAnim == &mid_airAttack && (SameRectP(mid_airAttack.GetCurrentFrame(), deavtice1) || SameRectP(mid_airAttack.GetCurrentFrame(), deavtice2))) {
					attackTrigger->active = false;
				}
				break;
			}
			else if (myDir == Direction::LEFT) {
				attackTrigger->body->SetTransform(pbody->body->GetPosition() + b2Vec2(1.5 - 1.2f, 0.8f), 0.0f);
				SDL_Rect deavtice1{ 1702 ,598 ,138 ,88 };
				SDL_Rect deavtice2{ 1842 ,598 ,138 ,88 };
				if (currentAnim == &mid_airAttack && (SameRectP(mid_airAttack.GetCurrentFrame(), deavtice1) || SameRectP(mid_airAttack.GetCurrentFrame(), deavtice2))) {
					attackTrigger->active = false;
				}
				break;
			}

		case PowerLvl::OP:
			if (myDir == Direction::RIGHT) {                                            // a 0.3 increase in the x axis
				op_attackTrigger->body->SetTransform(pbody->body->GetPosition() + b2Vec2(1.9f, 0.8f), 0.0f);
				SDL_Rect deavtice1{ 1741 ,865 ,198 ,133 };
				SDL_Rect deavtice2{ 1941 ,865 ,198 ,133 };
				if (currentAnim == &op_airAttack && (SameRectP(op_airAttack.GetCurrentFrame(), deavtice1) || SameRectP(op_airAttack.GetCurrentFrame(), deavtice2))) {
					op_attackTrigger->active = false;
				}
				break;
			}
			else if (myDir == Direction::LEFT) {
				op_attackTrigger->body->SetTransform(pbody->body->GetPosition() + b2Vec2(1 - 1.2f, 0.8f), 0.0f);
				SDL_Rect deavtice1{ 1741 ,865 ,198 ,133 };
				SDL_Rect deavtice2{ 1941 ,865 ,198 ,133 };
				if (currentAnim == &op_airAttack && (SameRectP(op_airAttack.GetCurrentFrame(), deavtice1) || SameRectP(op_airAttack.GetCurrentFrame(), deavtice2))) {
					op_attackTrigger->active = false;
				}
				break;
			}
		}

	}

}


// this functions is responsible for managing the animation logic of attacks
void Player::AttackingLogic() {
	switch (power) {
	case PowerLvl::NORMAL:
		op_attackTrigger->active = false;
		if (app->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN && isGrounded) {

			idleState = false;
			currentAnim = &groundAttack;
			Attacking = true;
		}
		else if (app->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN && isGrounded == false) {
			idleState = false;


			currentAnim = &airAttack;
			Attacking = true;
		}
		if (groundAttack.HasFinished()) {
			groundAttack.Reset();
			idleState = true;
			IdleTimer.Start();
			currentAnim = &idle;
			Attacking = false;
			groundBoost = false;
		}
		if (airAttack.HasFinished()) {
			airAttack.Reset();
			idleState = true;
			IdleTimer.Start();
			currentAnim = &idle;
			Attacking = false;
		}
		if (spearThrown.HasFinished() || spearThrown.loopCount > 1) {
			spearThrown.Reset();
			idleState = true;
			Attacking = false;
			thrown = false;
			thrownCooldown.Start();
			currentAnim = &idle;
			// When the spear is thrown in the animation it actually gets thrown//
			mySpear->position = position;
			mySpear->started = false;
			mySpear->daPlatform = false;
			mySpear->isPicked = false;
			mySpear->isSticked = false;

			SpearhasBeenThrown = true;

		}
		break;
	case PowerLvl::MID:
		attack = 2;
		op_attackTrigger->active = false;
		if (app->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN && isGrounded) {

			idleState = false;
			currentAnim = &mid_groundAttack;
			Attacking = true;
		}
		else if (app->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN && isGrounded == false) {
			idleState = false;

			currentAnim = &mid_airAttack;
			Attacking = true;
		}
		if (mid_groundAttack.HasFinished()) {
			mid_groundAttack.Reset();
			idleState = true;
			IdleTimer.Start();
			currentAnim = &idle;
			Attacking = false;
			groundBoost = false;
		}
		if (mid_airAttack.HasFinished()) {
			mid_airAttack.Reset();
			idleState = true;
			IdleTimer.Start();
			currentAnim = &idle;
			Attacking = false;
		}
		if (spearThrown.HasFinished() || spearThrown.loopCount > 1) {
			spearThrown.Reset();
			idleState = true;
			Attacking = false;
			thrown = false;
			thrownCooldown.Start();
			currentAnim = &idle;
			// When the spear is thrown in the animation it actually gets thrown//
			mySpear->position = position;
			mySpear->started = false;
			mySpear->daPlatform = false;
			mySpear->isPicked = false;
			mySpear->isSticked = false;

			SpearhasBeenThrown = true;
		}
		break;
	case PowerLvl::OP:
		attackTrigger->active = false;
		if (app->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN && isGrounded) {

			idleState = false;
			currentAnim = &op_groundAttack;
			Attacking = true;
		}
		else if (app->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN && isGrounded == false) {
			idleState = false;

			currentAnim = &op_airAttack;
			Attacking = true;
		}
		if (op_groundAttack.HasFinished()) {
			op_groundAttack.Reset();
			idleState = true;
			IdleTimer.Start();
			currentAnim = &idle;
			Attacking = false;
			groundBoost = false;
		}
		if (op_airAttack.HasFinished()) {
			op_airAttack.Reset();
			idleState = true;
			IdleTimer.Start();
			currentAnim = &idle;
			Attacking = false;
		}
		if (spearThrown.HasFinished() || spearThrown.loopCount > 1) {
			spearThrown.Reset();
			idleState = true;
			Attacking = false;
			thrown = false;
			thrownCooldown.Start();
			currentAnim = &idle;
			// When the spear is thrown in the animation it actually gets thrown//
			mySpear->position = position;
			mySpear->started = false;
			mySpear->daPlatform = false;
			mySpear->isPicked = false;
			mySpear->isSticked = false;

			SpearhasBeenThrown = true;
			break;
		}
	}
}

void Player::StartIFrames() {
	if (!iframes) {
		iframes = true;
		invicibilityTimer.Start();
		knockTimer.Start();
		
		b2Fixture* playerFixture = pbody->body->GetFixtureList();
		
		b2Filter iframePlayerFilter;
		iframePlayerFilter.categoryBits = PLAYER_CATEGORY_BIT;  
		iframePlayerFilter.maskBits = GROUND_CATEGORY_BIT;   // when invincible, player can only interact with ground    
		iframePlayerFilter.groupIndex = 0;

		playerFixture->SetFilterData(iframePlayerFilter);
	}
}

void Player::ManageInvencibility() {

	if (iframes && invicibilityTimer.ReadMSec() > INVINCIBILITY_MS) {
		iframes = false;
		b2Fixture* playerFixture = pbody->body->GetFixtureList();

		b2Filter iframePlayerFilter;
		iframePlayerFilter.categoryBits = PLAYER_CATEGORY_BIT;
		iframePlayerFilter.maskBits = GROUND_CATEGORY_BIT|ENEMY_CATEGORY_BIT; // now player will interact with ground and enemies
		iframePlayerFilter.groupIndex = 0;

		playerFixture->SetFilterData(iframePlayerFilter);

		invencibilityCounter = 0;
	}

}

void Player::InputControls(float dt) {
	if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
		//
	}
	if (isJumping == true)
	{
		// debe de haber algun problema , aqui le restamos a la velocidad en y pero el dt es mas grande cuanto menor los fps y esto hace que cauga menos/ salte mas
		gravity += 0.05f * dt;


	}


	if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && isJumping == false && isGrounded == true && pbody->body->GetLinearVelocity().y == 0 && app->titleS->options == false)
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

	if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && !(Attacking) && app->titleS->options == false) {

		IdleTimer.Start();
		if (isGrounded) {
			currentAnim = &longRun;
		}

		myDir = Direction::LEFT;
		movementx = -speed * dt;

	}


	if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && !(Attacking) && app->titleS->options == false) /*Ypu can move as long as youre not attacking on the ground*/ {

		IdleTimer.Start();
		if (isGrounded) {
			currentAnim = &longRun;
		}

		myDir = Direction::RIGHT;
		movementx = speed * dt;

		if (godMode == true) {
			movementx = speed * dt * 9;
		}
	}

	//--------------Attacking Logic-----------------//

	if (mySpear->isPicked && app->titleS->options == false)/*Can only attack if currently has the Spear*/ {
		AttackingLogic();
	}
	else if (app->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN && app->titleS->options == false) {
		_noSpearIcon = true;
		spear_icon_timer.Start();
		app->audio->PlayFx(noSpearEffect);
	}
	if (app->titleS->options == true) {
		app->physics->world->SetGravity({ 0,0 });
		gravity = 0;
	}
	else if (app->titleS->options == false)
	{
		app->physics->world->SetGravity({ 0,10 });

	}
}