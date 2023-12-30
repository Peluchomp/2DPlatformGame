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
	
	// the awake is only called for entities that are awaken with the manager
	_body = app->physics->CreateRectangle(position.x, position.y , 113, 30, bodyType::STATIC, ColliderType::PLATFORM,3000000.0f);
	_body->ctype = ColliderType::UNKNOWN;
	_body->listener = app->scene->player;
	_body->body->SetGravityScale(1);
	_body->myEntity = this;

	myBodies.Add(_body);

	plat_body = app->physics->CreateRectangleSensor(position.x, position.y -30, 115, 10, bodyType::STATIC, ColliderType::PLATFORM);
	plat_body->ctype = ColliderType::PLATFORM;
	plat_body->listener = app->scene->player;
	plat_body->body->SetGravityScale(1);
	plat_body->myEntity = this;
	
	myBodies.Add(plat_body);

	fallen = false;
	toDestroy = false;
	touched = false;

	num = 37;

	return true;
}

bool Chandelier::Start() {

	//initilize textures
	texture = app->tex->Load("Assets/Textures/chandelier.png");

	defaultAnim.PushBack({ 1,1,118,78 });
	defaultAnim.PushBack({ 121,1,118,78 });
	defaultAnim.PushBack({ 241,1,118,78 });

	defaultAnim.speed = 0.1f / 16;
	defaultAnim.loop = true;

	currentAnimation = &defaultAnim;

	return true;
}
bool Chandelier::PreUpdate(float dt)
{

	return  true;
}

bool Chandelier::Update(float dt)
{

	plat_body->body->SetTransform(_body->body->GetPosition() + b2Vec2(0, -0.2f), 0.0f);

	if (fallTimer.ReadSec() > 2 && touched) {
		_body->body->SetType(b2_dynamicBody);
		fallen = true;
		fallTimer.Start();
	}

	if(fallen && fallTimer.ReadMSec() > 400){
		if (_body->body->GetLinearVelocity().y == 0) {
			LOG("Destroy chandelier");

			_body->ctype == ColliderType::ENEMY_ATTACK;


			damage_body = app->physics->CreateRectangleSensor(position.x, position.y +40, 113, 30, bodyType::STATIC, ColliderType::ENEMY_ATTACK);
			damage_body->ctype = ColliderType::ENEMY_ATTACK;
			damage_body->listener = app->scene->player;
			damage_body->body->SetTransform(_body->body->GetPosition() + b2Vec2(0, -0.2f), 0.0f);
			myBodies.Add(damage_body);
			toDestroy = true;
		}
	
	}
	
	if (toDestroy) {
      	fallTimer.Start();
	}

	if (toDestroy && fallTimer.ReadMSec() < 400) {
		pendingToDestroy = true;

	}

	currentAnimation->Update();

	int x, y;
	_body->GetPosition(x, y);
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
	fallTimer.Start();
	touched = true;
}