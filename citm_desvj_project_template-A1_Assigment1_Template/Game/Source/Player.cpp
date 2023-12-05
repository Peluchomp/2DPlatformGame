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
	name.Create("player");

}

Player::~Player() {
	
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

	pbody = app->physics->CreateChain(position.x +35 , position.y ,PlayerCoords,8, bodyType::DYNAMIC);
	pbody->listener = this;
	
	pbody->ctype = ColliderType::PLAYER;

	attackTrigger = app->physics->CreateRectangleSensor(position.x + 110, position.y+40, 60, 70, bodyType::STATIC);
	attackTrigger->listener = this; // CHANGE to enemies
	attackTrigger->ctype = ColliderType::PLAYER_ATTACK;


	/*plegs = app->physics->CreateCircle(position.x + 16, position.y + 30, 10, bodyType::STATIC);
	plegs->listener = this;
	plegs->ctype = ColliderType::UNKNOWN;*/
	
	currentAnim = &epicSpawn;
	currentSpawnAnim = &epicSpawn;
	//idleState = true;
	mySpear->currentAnim = &mySpear->form1Anim;
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


		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && !(Attacking)) {

			IdleTimer.Start();
			if (isGrounded) {
				currentAnim = &longRun;
			}

			myDir = Direction::LEFT;
			movementx = -speed * dt;
		}

		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && !(Attacking)) /*Ypu can move as long as youre not attacking on the ground*/ {

			IdleTimer.Start();
			if (isGrounded) {
				currentAnim = &longRun;
			}

			myDir = Direction::RIGHT;
			movementx = speed * dt;
		}
		//--------------Attacking Logic-----------------//

		if (app->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN && isGrounded) {
			
			idleState = false;
			currentAnim = &groundAttack;
			Attacking = true;
		}
		else if (app->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN && isGrounded == false) {
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
		/*if (Attacking && isGrounded && groundAttack.numOpportunities > 1 && !groundBoost) {
			movementx += 40;
			groundBoost = true;
		}*/
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
		mousex -= app->render->camera.x;
		mousey -= app->render->camera.y;
		SDL_Point center{ position.x,position.y };
		SDL_Rect  perim{ position.x,position.y,0,0 };
		delta_x = position.x - mousex/2;
		delta_y = position.y - mousey/2;

		angle_deg = (atan2(delta_y, delta_x) * 180.0000) / 3.1416;

	
	currentAnim->Update();
	spawnFire.Update();
	
	app->render->DrawCircle(position.x+35,position.y-20, 20, 255, 255, 0, 255);

	if (myDir == Direction::RIGHT) {
		if(spawning){ app->render->DrawTexture(texture, position.x , position.y - 100, false, &currentAnim->GetCurrentFrame()); }
		else { app->render->DrawTexture(texture, position.x - 36, position.y - 40, false, &currentAnim->GetCurrentFrame()); }
	}
	else {
		if (spawning) { app->render->DrawTexture(texture, position.x, position.y - 100, false, &currentAnim->GetCurrentFrame()); }
		else { app->render->DrawTexture(texture, position.x - 36, position.y - 40, true, &currentAnim->GetCurrentFrame()); }
	}

	if (SpearhasBeenThrown) {
		app->render->DrawTexture(mySpear->texture, mySpear->position.x, mySpear->position.y,false, &mySpear->currentAnim->GetCurrentFrame());
	}

	if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT && mySpear->isPicked == true) //en vez de w usamos app->input->GetMouseButtonDown(0) == KEY_REPEAT
	{
		IdleTimer.Start();
		app->render->DrawTexture(texture, pbody->body->GetTransform().p.x, pbody->body->GetTransform().p.y - 16, false, 0, 0, angle_deg);
	}
	
	if (app->input->GetKey(SDL_SCANCODE_W) == KEY_UP) 
	{
		if (mySpear->isPicked == true && thrownCooldown.ReadSec() > 2) {
			if (thrown == false ) {
				currentAnim = &spearThrown;
				idleState = false;
				Attacking = true;
				thrown = true;
				
			}

			
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

	// ---------------Orb stuf----------------//
	orbMeter = { 50 - (app->render->camera.x/2), 20, orbs*10, 15 };
	app->render->DrawRectangle(orbMeter, 50, 0, 140, 255);

	// Method that manages the logic of the attack hitbox
	AttackHitBoxManagement();
	
	return true;
}

bool Player::CleanUp()
{

	return true;
}

void Player::OnCollision(PhysBody* physA, PhysBody* physB) {

	if (dead == false && physB->active) {

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
		case ColliderType::ORB:
			LOG("Player touched an orb");
			orbs++;
			physB->active = false;
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

		orbs = 0;
	}
}

bool Player::LoadState(pugi::xml_node node) {

	position.x = node.child("position").attribute("x").as_int();
	position.y = node.child("position").attribute("y").as_int();

	return true;
}

// L14: TODO 8: Create a method to save the state of the renderer
// using append_child and append_attribute
bool Player::SaveState(pugi::xml_node node) {

	pugi::xml_node Node = node.append_child("position");
	Node.append_attribute("x").set_value(position.x);
	Node.append_attribute("y").set_value(position.y);

	return true;
}

void Player::LoadAnimations() {

	for (pugi::xml_node node = parameters.child("animations").child("idle").child("frame"); node != NULL; node = node.next_sibling("frame")) {

		idle.PushBack({ node.attribute("x").as_int() , node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() });
		idle.speed = parameters.child("animations").child("idle").child("speed").attribute("value").as_float() / 16;
		//idle.loop = parameters.child("animations").child("idle").child("loop").attribute("value").as_bool();
	}
	for (pugi::xml_node node = parameters.child("animations").child("longIdle1").child("frame"); node != NULL; node = node.next_sibling("frame")) {

		longIdle1.PushBack({ node.attribute("x").as_int() , node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() });
		longIdle1.speed = parameters.child("animations").child("longIdle1").child("speed").attribute("value").as_float() / 16;
		longIdle1.loop = parameters.child("animations").child("longIdle1").child("loop").attribute("value").as_bool();
	}
	for (pugi::xml_node node = parameters.child("animations").child("longIdle2").child("frame"); node != NULL; node = node.next_sibling("frame")) {

		longIdle2.PushBack({ node.attribute("x").as_int() , node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() });
		longIdle2.speed = parameters.child("animations").child("longIdle2").child("speed").attribute("value").as_float() / 16;
		//longIdle2.loop = parameters.child("animations").child("longIdle2").child("loop").attribute("value").as_bool();

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

		groundAttack.PushBack({ node.attribute("x").as_int() , node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() }, node.attribute("opportunity").as_bool());
		groundAttack.speed = parameters.child("animations").child("groundAttack").child("speed").attribute("value").as_float() / 16;
		groundAttack.loop = parameters.child("animations").child("groundAttack").child("loop").attribute("value").as_bool();
	}

	groundAttack.opportunityFrame = 2;
	groundAttack.opportunityKey = SDL_SCANCODE_M;
	groundAttack.opportunityWindow = 0.2f;

	airAttack.loop = false;
	airAttack.speed = 0.19f / 16;
	airAttack.opportunityKey = SDL_SCANCODE_M;
	airAttack.opportunityWindow = 0.15f;
	airAttack.opportunityFrame = 2;


	for (pugi::xml_node node = parameters.child("animations").child("airAttack").child("frame"); node != NULL; node = node.next_sibling("frame")) {

		airAttack.PushBack({ node.attribute("x").as_int() , node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() }, node.attribute("opportunity").as_bool());
		airAttack.speed = parameters.child("animations").child("airAttack").child("speed").attribute("value").as_float() / 16;
		airAttack.loop = parameters.child("animations").child("airAttack").child("loop").attribute("value").as_bool();
	}
	for (pugi::xml_node node = parameters.child("animations").child("fall").child("frame"); node != NULL; node = node.next_sibling("frame")) {

		Fall.PushBack({ node.attribute("x").as_int() , node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() });
		Fall.speed = parameters.child("animations").child("fall").child("speed").attribute("value").as_float() / 16;

	}
	for (pugi::xml_node node = parameters.child("animations").child("spawnFire").child("frame"); node != NULL; node = node.next_sibling("frame")) {

		spawnFire.PushBack({ node.attribute("x").as_int() , node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() });
		spawnFire.speed = parameters.child("animations").child("spawnFire").child("speed").attribute("value").as_float() / 16;
	}
	for (pugi::xml_node node = parameters.child("animations").child("spearThrown").child("frame"); node != NULL; node = node.next_sibling("frame")) {

		spearThrown.PushBack({ node.attribute("x").as_int() , node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() });
		spearThrown.speed = parameters.child("animations").child("spearThrown").child("speed").attribute("value").as_float() / 16;
		spearThrown.loop = parameters.child("animations").child("spearThrown").child("loop").attribute("value").as_bool();
	}
	for (pugi::xml_node node = parameters.child("animations").child("jump").child("frame"); node != NULL; node = node.next_sibling("frame")) {

		Jump.PushBack({ node.attribute("x").as_int() , node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() });
		Jump.speed = parameters.child("animations").child("jump").child("speed").attribute("value").as_float() / 16;
		Jump.loop = parameters.child("animations").child("jump").child("loop").attribute("value").as_bool();
	}

	for (pugi::xml_node node = parameters.child("animations").child("epicSpawn").child("frame"); node != NULL; node = node.next_sibling("frame")) {

		epicSpawn.PushBack({ node.attribute("x").as_int() , node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() });
		epicSpawn.speed = parameters.child("animations").child("epicSpawn").child("speed").attribute("value").as_float() / 16;
		epicSpawn.loop = false;
	}
	//epicSpawn.speed = 0.13f/16;


	for (pugi::xml_node node = parameters.child("animations").child("quickSpawn").child("frame"); node != NULL; node = node.next_sibling("frame")) {

		quickSpawn.PushBack({ node.attribute("x").as_int() , node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() });
		quickSpawn.speed = parameters.child("animations").child("quickSpawn").child("speed").attribute("value").as_float() / 16;
		quickSpawn.loop = false;
	}
}


void Player::AttackHitBoxManagement() {

	/*b2Vec2 v(PIXEL_TO_METERS(position.x + 110), PIXEL_TO_METERS(position.y + 40));
	attackTrigger->body->SetTransform(v, 0  );*/

	/*if (Attacking) {
		attackTrigger->active = true;
	}
	else {
		attackTrigger->active = false;
	}*/



}