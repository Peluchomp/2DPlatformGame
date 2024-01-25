#include "Aelfric.h"
#include "Source/App.h"
#include "Source/Textures.h"
#include "Source/Audio.h"
#include "Source/Input.h"
#include "Source/Render.h"
#include "Source/Scene.h"
#include "Source/Log.h"
#include "Source/Point.h"
#include "Source/Physics.h"
#include "EvilSpin.h"
#include <cstdlib>
#include <ctime>

int getRandomNumber(int min, int max) {
	// Seed the random number generator with the current time
	std::srand(static_cast<unsigned int>(std::time(0)));

	// Generate a random number between min and max (inclusive)
	return min + std::rand() % (max - min + 1);
}

Aelfric::Aelfric() : Entity(EntityType::AELFRIC)
{
	name.Create("aelfric");
}

Aelfric::~Aelfric() {}

bool Aelfric::Awake() {

	// the awake is only called for entities that are awaken with the manager



	return true;
}

bool Aelfric::Start() {

	//initilize textures
	texture = app->tex->Load( parameters.child("texture").attribute("path").as_string());
	position.x = parameters.child("position").attribute("x").as_int();
	position.y = parameters.child("position").attribute("y").as_int();
	attackChangeTimer.Start();

	spinningFx = app->audio->LoadFx( parameters.child("spinning_spear").attribute("audiopath").as_string());
	thunderFx = app->audio->LoadFx(parameters.child("thunder").attribute("audiopath").as_string());
	teleportFx = app->audio->LoadFx(parameters.child("teleport").attribute("audiopath").as_string());

	ogPos = position;

 	for (pugi::xml_node node = parameters.child("animations").child("walking").child("frame"); node != NULL; node = node.next_sibling("frame")) {

		walkingAnim.PushBack({ node.attribute("x").as_int() , node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() });
		walkingAnim.speed = parameters.child("animations").child("walking").child("speed").attribute("value").as_float() / 16;
	}
	for (pugi::xml_node node = parameters.child("animations").child("magic").child("frame"); node != NULL; node = node.next_sibling("frame")) {

		magicAnim.PushBack({ node.attribute("x").as_int() , node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() });
		magicAnim.speed = parameters.child("animations").child("magic").child("speed").attribute("value").as_float() / 16;
	}

	currentAnimation = &walkingAnim;

	_body = app->physics->CreateCircle(position.x, position.y, 30, bodyType::DYNAMIC, ColliderType::ENEMY);
	_body->ctype = ColliderType::ENEMY;
	_body->body->SetGravityScale(1);
	_body->listener = this;

	myDir = Direction::RIGHT;
	ChangePosTimer.Start();

	hp = 20;

	MrSpear.texture = texture; MsSpear.texture = texture;
	MrSpear.pbody = app->physics->CreateRectangle(position.x, position.y, 10, 65, bodyType::DYNAMIC, ColliderType::PHYS2, 0.1);
	MrSpear.pbody->ctype = ColliderType::ENEMY_ATTACK;
	MrSpear.pbody->body->SetGravityScale(0);

	MsSpear.pbody = app->physics->CreateRectangle(position.x, position.y, 10, 65, bodyType::DYNAMIC, ColliderType::PHYS2, 0.1);
	MsSpear.pbody->body->SetGravityScale(0);
	MsSpear.pbody->ctype = ColliderType::ENEMY_ATTACK;


	CreateSpears();
	_detectionBody = app->physics->CreateRectangleSensor(position.x, position.y, 250, 200, bodyType::DYNAMIC, ColliderType::ENEMY);
	myBodies.Add(_detectionBody);
	_detectionBody->listener = this;
	_detectionBody->body->SetGravityScale(0);

	currentAttack = GROUND_SPEARS;
	floorSpearTimer.Start();

	ogP1 = MrSpear.pbody->body->GetPosition(); 
	ogP2 = MrSpear.pbody->body->GetPosition();
	
	return true;
}

bool Aelfric::PreUpdate(float dt)
{
	if (destroySpears) { DestroyFloatingSpears(); }
	return  true;
}

bool Aelfric::Update(float dt)
{
	if (!startFight && app->scene->bossZone == true){
		attackChangeTimer.Start();
		startFight = true;
		ogTransform = _body->body->GetPosition();
	}

	
	
	int R = 255, G = 255, B = 255;
	if (_body->active && app->scene->bossZone == true) {
		_body->GetPosition(position.x, position.y);

		/*Select a random attack*/
		if (attackChangeTimer.ReadSec() > 15 && groundPhase == true) {
			attackChangeTimer.Start();
			startedSpin = false; startedThunder = false;

			int id = getRandomNumber(1, 2);
			switch (id) {
			case(1):
				currentAttack = SPIN;
				groundPhase = false;
				break;
			case(2):
				currentAttack = THUNDERS;
				groundPhase = false;
				break;
			}

		}
		if (attackChangeTimer.ReadSec() > 10 && groundPhase == false) {
			groundPhase = true;
			currentAttack = GROUND_SPEARS;
			Teleport();
			CreateSpears();
			attackChangeTimer.Start();
		
		}
		
		
		if (currentAttack == SPIN ) {
			
			SpinAttackLogic();

		}
		else if (currentAttack == THUNDERS) {
			ThunderLogic();
		}

		if (currentAttack == GROUND_SPEARS) {
			b2Vec2 Velocity;

			
			if (floorSpearTimer.ReadSec() > floorSpearWait) {
				FloorSpears* fs = (FloorSpears*) app->entityManager->CreateEntity(EntityType::FLOORSPEAR);
				fs->Awake();
				fs->SetSpeed(4);
				floorSpearTimer.Start();
				floorSpearWait = 1.5f / getRandomNumber(1, 3);
			}


			if (myDir == Direction::RIGHT) {

				Velocity.x = 2; Velocity.y = _body->body->GetLinearVelocity().y;

			}
			else if (myDir == Direction::LEFT) {
				Velocity.x = -2; Velocity.y = _body->body->GetLinearVelocity().y;

			}

			if (ChangePosTimer.ReadSec() > 2) {

				if (getRandomNumber(0, 1) == 0) {
					myDir = Direction::RIGHT;
				}
				else {
					myDir = Direction::LEFT;
				}

			}


			_body->body->SetLinearVelocity(Velocity);
			_body->body->SetLinearVelocity(Velocity);

			


			




			

			if (hurt == true) {
				if (hurtTimer.ReadMSec() < 800) {
					G = 0; B = 0;
				}
				else {
					hurt = false;
				}

			}


			// The spears spin if the player enters into the dtection collider, after 1.5s they stpo spinning
			if (defending && defendTimer.ReadMSec() > 1500 && !destroySpears) {
				defending = false;
				revol1->SetMotorSpeed(0);
				revol2->SetMotorSpeed(0);

				// reset orientation
				b2Vec2 pos1 = MrSpear.pbody->body->GetPosition(); pos1.y = ogP1.y;
				b2Vec2 pos2 = MrSpear.pbody->body->GetPosition(); pos1.y = ogP2.y;

				MrSpear.pbody->body->SetTransform(pos1, 0); MrSpear.pbody->body->SetTransform(pos2, 0);
			}

			int sp1Posx;
			int sp1Posy;
			MrSpear.pbody->GetPosition(sp1Posx, sp1Posy);
			int sp2Posx;
			int sp2Posy;
			MsSpear.pbody->GetPosition(sp2Posx, sp2Posy);



			_detectionBody->body->SetTransform(_body->body->GetPosition(), 0.0f);
			MsSpear.pbody->body->SetTransform(MsSpear.pbody->body->GetPosition(), MrSpear.pbody->GetRotation());



			

			if (!destroySpears) {
				SDL_Rect spearRect = { 560,1,17,85 };
				app->render->DrawTexture(texture, sp1Posx - 8, sp1Posy - 30, false, &spearRect, 255, 1, 255, 255, 255, MrSpear.pbody->GetRotation());
				app->render->DrawTexture(texture, sp2Posx - 8, sp2Posy - 30, false, &spearRect, 255, 1, 255, 255, 255, MsSpear.pbody->GetRotation());
			}
		}
		currentAnimation->Update();
		if (myDir == Direction::LEFT) {
			app->render->DrawTexture(texture, position.x - 40, position.y - 35, true, &currentAnimation->GetCurrentFrame(), 255, 1, R, G, B);
		}
		else if (myDir == Direction::RIGHT) {
			app->render->DrawTexture(texture, position.x - 60, position.y - 35, false, &currentAnimation->GetCurrentFrame(), 255, 1, R, G, B);
		}
	}


	return true;
}

bool Aelfric::CleanUp()
{
	return true;
}

void Aelfric::OnCollision(PhysBody* physA, PhysBody* physB) {


	switch (physB->ctype)
	{
	case ColliderType::ENEMY:
		LOG("Collision ENEMY");

		break;

	case ColliderType::PLAYER:
		LOG("Player touched Checkpoint");

		if (revol1 != nullptr && revol2 != nullptr) {
			revol1->SetMotorSpeed(10);
			revol2->SetMotorSpeed(10);
			defendTimer.Start();
			defending = true;
		}
		break;
	case ColliderType::ENEMY_ATTACK:
		LOG("erpt");
		if (physB != MrSpear.pbody && physB != MsSpear.pbody) {
			hp -= 3;
			destroySpears = true;
			hurt = true;
			hurtTimer.Start();
		}
		break;
	case ColliderType::PLAYER_ATTACK:
		if (physB->active && physA == _body) {
			LOG("erpt");
			hp -= app->scene->player->attack;
			hurt = true;
			hurtTimer.Start();
		}
		break;

	}
}

void Aelfric::DestroyFloatingSpears() {

	app->entityManager->destroyJoints.Add(revol1);
	app->entityManager->destroyJoints.Add(revol2);

	app->entityManager->destroyJoints.Add(MrSpear.revol);
	app->entityManager->destroyJoints.Add(MsSpear.revol);
	MrSpear.pbody->active = false; MsSpear.pbody->active = false;
	myBodies.Add(MrSpear.pbody); myBodies.Add(MsSpear.pbody);
}

void Aelfric::CreateSpears() {
	
	destroySpears = false;
	
	MrSpear.pbody->active = true; MsSpear.pbody->active = true;

	float relativeAnchorX_B = 0.5f;  // Offset to the right
	float relativeAnchorY_B = -0.5f;

	float relativeAnchorX_C = -0.5f;  // Offset to the left
	float relativeAnchorY_C = -0.5f;

	// These joints make the spears spin around themselves
	revol1 = app->physics->CreateRevoluteJoint(_body, MrSpear.pbody, relativeAnchorX_B, relativeAnchorY_B, 0, 200);
	revol2 = app->physics->CreateRevoluteJoint(_body, MsSpear.pbody, relativeAnchorX_C, relativeAnchorY_C, 0, 5.0f);


	MrSpear.pbody->body->SetTransform(_body->body->GetPosition() + b2Vec2(1.0f, 0.0f), MrSpear.pbody->GetRotation());
	MsSpear.pbody->body->SetTransform(_body->body->GetPosition() + b2Vec2(1.0f, 0.0f), MsSpear.pbody->GetRotation());

	// These joints bind the spears to father Aelfric
	MsSpear.revol = app->physics->CreateRevoluteJoint(_body, MsSpear.pbody, 2, 0, 0, 5);
	MrSpear.revol = app->physics->CreateRevoluteJoint(_body, MrSpear.pbody, -2, 0, 0, 5);
	MrSpear.pbody->body->SetAngularDamping(20);
	MsSpear.pbody->body->SetAngularDamping(20);
}

bool Aelfric::PostUpdate() {

	if (app->scene->bossZone == true) {

		healthBar = { 520 - (app->render->camera.x / 2), 20 - (app->render->camera.y / 2), 20, 7 * hp };
		app->render->DrawRectangle(SDL_Rect{ 520 - (app->render->camera.x / 2), 20 - (app->render->camera.y / 2), 20, 140 }, 78, 0, 0, 255);
		app->render->DrawRectangle(healthBar, 27, 210, 152, 255);

	}
	if (currentAttack != GROUND_SPEARS && MrSpear.pbody != nullptr && MsSpear.pbody != nullptr) {

		
	}
	return true;
}

void Aelfric::Teleport() {
	app->audio->PlayFx(teleportFx);

	if (currentAttack == GROUND_SPEARS) /*Teleport to ground, start walking and reset tranform*/ {

		currentAnimation = &walkingAnim;
		_body->body->SetTransform(ogTransform, 0);

	}
	else {
		currentAnimation = &magicAnim;
		int id = getRandomNumber(1, 2);
		if (id == 1) {
			myDir = Direction::LEFT; /*He stands at the right, facing left*/
			// enter magical animation

			destroySpears = true;
			_body->body->SetTransform(b2Vec2(PIXEL_TO_METERS(116 * 40), PIXEL_TO_METERS(18 * 40)), 0);
		}
		else if (id == 2) {
			myDir = Direction::RIGHT;
			// enter magical animation

			destroySpears = true;
			_body->body->SetTransform(b2Vec2(PIXEL_TO_METERS(109 * 40), PIXEL_TO_METERS(18 * 40)), 0);
		}
	}
}

void Aelfric::SpinAttackLogic() {
	if (!startedSpin) {
		Teleport();
		_body->body->SetLinearVelocity(b2Vec2_zero);
		startedSpin = true;
	}

	if (spinTimeDecided == false) {
		// throw first rotating Spear
		EvilSpin* Es = (EvilSpin*)app->entityManager->CreateEntity(EntityType::EVILSPIN);
		Es->position = position;
		Es->position.y += 50;
		Es->Awake();
		app->audio->PlayFx(spinningFx);
		spinTimeDecided = true;
		floorSpearTimer.Start();
		int id = getRandomNumber(1, 2);
		if (id == 1) {
			floorSpearWait = 0.5f;
		}
		else {
			floorSpearWait = 2;
		}
	}

	if (floorSpearTimer.ReadSec() > 1) {
		int randodist = getRandomNumber(1, 3);
		EvilSpin* Es = (EvilSpin*)app->entityManager->CreateEntity(EntityType::EVILSPIN);
		Es->Awake();
		Es->position = position;
		Es->position.y += 50;
		app->audio->PlayFx(spinningFx);
		switch (randodist) {
		case(1): Es->SetDistance(180); break;

		case(2):Es->SetDistance(80); break;

		case(3):Es->SetDistance(250); break;
		}

		Es->Awake();
		floorSpearTimer.Start();


	}
}

void Aelfric::ThunderLogic() {

	if (!startedThunder) {
		Teleport();
		_body->body->SetLinearVelocity(b2Vec2_zero);
		startedThunder = true;

		int id = getRandomNumber(1, 4);

		EvilSpearLightning* esL =  (EvilSpearLightning*)app->entityManager->CreateEntity(EntityType::EVILSPEARLIGHTNING);
		app->audio->PlayFx(thunderFx);
		esL->Awake();
		switch (id) {
		case(1):
			esL->SetSpeed(0.5f);
			break;
		case(2):
			esL->SetSpeed(0.8f);
			break;
		case(3):
			esL->SetSpeed(1.2f);
			break;
		case(4):
			esL->SetSpeed(0.2f);
		}

	}

	
}