#include "Chandelier.h"
#include "Source/App.h"
#include "Source/Textures.h"
#include "Source/Audio.h"
#include "Source/Input.h"
#include "Source/Render.h"
#include "Source/Scene.h"
#include "Source/Log.h"
#include "Source/Point.h"
#include "Source/Physics.h"

Chandelier::Chandelier() : Entity(EntityType::CHANDELIER)
{
	name.Create("chandelier");
}


Chandelier::Chandelier(iPoint ogPosition) {
	position = ogPosition;
}

Chandelier::~Chandelier() {}

bool Chandelier::Awake() {

	ogPos = position;
	
	if (parameters.attribute("type").as_int() == 0) {
		myType = ChandelierType::STATIONARY;
	}
	else { myType = ChandelierType::PENDULUM; }
	

	// the awake is only called for entities that are awaken with the manager
	_body = app->physics->CreateRectangle(position.x, position.y , 113, 30, bodyType::DYNAMIC, ColliderType::PHYSIC_OBJ,3);
	_body->ctype = ColliderType::PHYSIC_OBJ;
	//_body->listener = app->scene->player;
	_body->body->SetGravityScale(0);
	_body->myEntity = this;

	myBodies.Add(_body);

	plat_body = app->physics->CreateRectangle(position.x, position.y -30, 115, 10, bodyType::STATIC, ColliderType::PLATFORM);
	plat_body->ctype = ColliderType::PLATFORM;
	plat_body->listener = app->scene->player;
	plat_body->body->SetGravityScale(0);
	plat_body->myEntity = this;

	plat_sensor = app->physics->CreateRectangleSensor(position.x, position.y - 30, 115, 10, bodyType::STATIC, ColliderType::PLATFORM);
	plat_sensor->ctype = ColliderType::PLATFORM;
	plat_sensor->listener = app->scene->player;
	plat_sensor->body->SetGravityScale(0);
	plat_sensor->myEntity = this;
	
	myBodies.Add(plat_body);
	myBodies.Add(plat_sensor);

	fallen = false;
	toDestroy = false;
	touched = false;

	jointBody = app->physics->CreateCircle(position.x, position.y, 5, bodyType::STATIC, ColliderType::UNKNOWN,true);

	jointBody->body->SetTransform(_body->body->GetPosition() + b2Vec2(0., -2.9f), 0.0f);


	

	if (myType == ChandelierType::PENDULUM) {

		revol = app->physics->CreateRevolutionJoint(jointBody, _body);
		revolTimer.Start();
		if (parameters.attribute("dir").as_string() == "left") {
			startingDir = Direction::LEFT;
			revol->SetMotorSpeed(-40);  // Adjust as needed

		}
		else {
			startingDir = Direction::RIGHT;
			revol->SetMotorSpeed(40);  // Adjust as needed
		}
	}

	return true;
}

bool Chandelier::Start() {

	//initilize textures
	texture = app->entityManager->chandelier_tex;
	
	// CHANGE
	defaultAnim.PushBack({ 1,1,118,78 });
	defaultAnim.PushBack({ 121,1,118,78 });
	defaultAnim.PushBack({ 241,1,118,78 });

	stationaryAnim.PushBack({ 1,81,118,78 });
	stationaryAnim.PushBack({ 121,81,118,78 });
	stationaryAnim.PushBack({ 241, 81,118,78 });

	defaultAnim.speed = 0.1f / 16;
	defaultAnim.loop = true;
	stationaryAnim.speed = 0.1f / 16;
	stationaryAnim.loop = true;

	if(myType == ChandelierType::STATIONARY) currentAnimation = &stationaryAnim;

	else { currentAnimation = &defaultAnim; }

	return true;
}
bool Chandelier::PreUpdate(float dt)
{

	return  true;
}

bool Chandelier::Update(float dt)
{

	int x, y;
	_body->GetPosition(x, y);

	plat_body->body->SetTransform(_body->body->GetPosition() + b2Vec2(0, -0.2f), 0.0f);
	plat_sensor->body->SetTransform(_body->body->GetPosition() + b2Vec2(0, -0.3f), 0.0f);

	if (fallTimer.ReadMSec() > 500 && touched && !destroyedJoint) {
		_body->body->SetType(b2_dynamicBody);
		_body->body->SetGravityScale(1);
		if (myType == ChandelierType::PENDULUM) {
			jointBody->body->SetType(b2_dynamicBody);
			jointBody->body->SetGravityScale(1);
			app->physics->world->DestroyJoint(revol);
		}
		destroyedJoint = true;
		fallen = true;
		fallTimer.Start();
		damage_body = app->physics->CreateRectangleSensor(position.x, position.y + 40, 113, 30, bodyType::STATIC, ColliderType::ENEMY_ATTACK);
		damage_body->ctype = ColliderType::ENEMY_ATTACK;
		damage_body->listener = app->scene->player;
		damage_body->body->SetTransform(_body->body->GetPosition() + b2Vec2(0, -0.2f), 0.0f);
		myBodies.Add(damage_body);
	}
	if(fallen){ damage_body->body->SetTransform(_body->body->GetPosition() + b2Vec2(0, +0.2f), 0.0f); }

	if(fallen && fallTimer.ReadMSec() > 600){
		if (_body->body->GetLinearVelocity().y < 45) {
			LOG("Destroy chandelier");

			_body->ctype == ColliderType::ENEMY_ATTACK;


			
			toDestroy = true;
		}
	
	}
	
	if (toDestroy) {
      	fallTimer.Start();
		toDestroy = false;
		destroying = true;
	}

	if (destroying && fallTimer.ReadMSec() > 400) {
		pendingToDestroy = true;

	}

	_body->body->SetAngularDamping(0.1f);
	_body->body->SetAngularVelocity(0.0f);

	if (myType == ChandelierType::PENDULUM) {
		if (revolTimer.ReadMSec() > 1400 && !fallen) {
			revolTimer.Start();


			if (revol->GetMotorSpeed() > 0) {


				myDir = Direction::LEFT;
				revol->SetMotorSpeed(-40);  // Adjust as needed

			}
			else {
				myDir = Direction::RIGHT;
				revol->SetMotorSpeed(40);  // Adjust as needed
				app->scene->player->movementx -= 10;
			}
		}
		if (fallen) { revol->SetMotorSpeed(0); }
	}

	plat_sensor->collider = { x , y -15 , 115, 10 };
	if (app->physics->debug) {
		app->render->DrawRectangle(plat_sensor->collider, 200, 200, 200, 255, true);
	}
	
	
		bool playerOnTop = false;
		if (plat_sensor->Intersects(&app->scene->player->pbody->collider)) {
			LOG("Player on top");
			playerOnTop = true;
		}
		if (playerOnTop) {
			if (myType == ChandelierType::PENDULUM) {
				if (myDir == Direction::LEFT) {
					app->scene->player->pbody->body->SetTransform(app->scene->player->pbody->body->GetPosition() + b2Vec2(0.025f, 0), 0);
				}
				else if (myDir == Direction::RIGHT) {
					app->scene->player->pbody->body->SetTransform(app->scene->player->pbody->body->GetPosition() + b2Vec2(-0.025f, 0), 0);
				}
			}
			else if (myType == ChandelierType::STATIONARY) {
				PlayerStandingOnME();
			}
		}
	

	currentAnimation->Update();

	int x1, y1, x2, y2, x3, y3;
	_body->GetPosition(x1, y1);
	jointBody->GetPosition(x2, y2);
	x3 = x2 ; y3 = y2;

	b2Vec2 chain =  b2Vec2(x1-x2, y1-y2);
	if(myType == ChandelierType::STATIONARY){ chain = b2Vec2(x1 - x2 +55, y1 - y2); }
	b2Vec2 chain2 = b2Vec2(x1 - x3 +110, y1 - y3);

	
	for (int i = 0; i < 10; ++i) {
		if (myType == ChandelierType::PENDULUM) {
			b2Vec2 current = b2Vec2(x2 + chain.x * i / 10, y2 + chain.y * i / 10);
			app->render->DrawTexture(texture, current.x, current.y, false, &_chain1);

			b2Vec2 current2 = b2Vec2(x3 + chain2.x * i / 10, y3 + chain2.y * i / 10);
			app->render->DrawTexture(texture, current2.x, current2.y, false, &_chain1);
		}
		else if (myType == ChandelierType::STATIONARY) {
			b2Vec2 current = b2Vec2(x2 + chain.x * i / 10, y2 + chain.y * i / 10);
			app->render->DrawTexture(texture, current.x, current.y, false, &_chain2);

			
		}

	}

	
	app->render->DrawTexture(texture, x, y -25, false, &currentAnimation->GetCurrentFrame());

	return true;
}

bool Chandelier::CleanUp()
{
	return true;
}

void Chandelier::OnCollision(PhysBody* physA, PhysBody* physB) {


	switch (physB->ctype)
	{
	case ColliderType::ENEMY:
		LOG("Collision ENEMY");

		break;

	case ColliderType::PLAYER:
		LOG("Player touched chandelier");
		break;


	}
}

void Chandelier::PlayerStandingOnME() {
	if (touched == false) {
		fallTimer.Start();
		touched = true;
	}
}