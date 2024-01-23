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


 	for (pugi::xml_node node = parameters.child("animations").child("walking").child("frame"); node != NULL; node = node.next_sibling("frame")) {

		walkingAnim.PushBack({ node.attribute("x").as_int() , node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() });
		walkingAnim.speed = parameters.child("animations").child("walking").child("speed").attribute("value").as_float() / 16;
	}

	currentAnimation = &walkingAnim;

	_body = app->physics->CreateCircle(position.x, position.y, 30, bodyType::DYNAMIC, ColliderType::ENEMY);
	_body->ctype = ColliderType::ENEMY;
	_body->body->SetGravityScale(1);
	_body->listener = this;

	_body2 = app->physics->CreateCircle(position.x, position.y, 30, bodyType::DYNAMIC, ColliderType::PHYS2,true);
	_body2->ctype = ColliderType::ENEMY;
	_body2->body->SetGravityScale(1);
	_body3 = app->physics->CreateCircle(position.x , position.y, 30, bodyType::DYNAMIC, ColliderType::PHYS2,true);
	_body3->ctype = ColliderType::ENEMY;
	_body3->body->SetGravityScale(0);
	

	myDir = Direction::RIGHT;
	ChangePosTimer.Start();

	


	MrSpear.texture = texture; MsSpear.texture = texture;
	MrSpear.pbody = app->physics->CreateRectangle(30, 0, 16, 50, bodyType::DYNAMIC, ColliderType::PHYSIC_OBJ, 0.1);
	MrSpear.pbody->ctype = ColliderType::ENEMY_ATTACK;
	MrSpear.pbody->body->SetGravityScale(0);

	MsSpear.pbody = app->physics->CreateRectangle(200, 0, 16, 50, bodyType::DYNAMIC, ColliderType::PHYSIC_OBJ, 0.1);
	MsSpear.pbody->body->SetGravityScale(0);
	MsSpear.pbody->ctype = ColliderType::ENEMY_ATTACK;

	/*MrSpear.rotationB = app->physics->CreateCircle(MrSpear.pbody->body->GetPosition().x, MrSpear.pbody->body->GetPosition().y, 5, bodyType::DYNAMIC, ColliderType::PHYSIC_OBJ, false);
	MsSpear.rotationB = app->physics->CreateCircle(MrSpear.pbody->body->GetPosition().x, MrSpear.pbody->body->GetPosition().y, 5, bodyType::DYNAMIC, ColliderType::PHYSIC_OBJ, true);
	MrSpear.rotationB->body->SetGravityScale(0); MsSpear.rotationB->body->SetGravityScale(0);*/


	float rightAnchorX = 2;  // Adjust as needed
	float rightAnchorY = 0.0f;

	// Left side for MsSpear
	float leftAnchorX = -2;  // Adjust as needed
	float leftAnchorY = 0.0f;


	// revolute joints
	MrSpear.revol = app->physics->CreateRevoluteJoint(MrSpear.pbody, _body2, rightAnchorX, rightAnchorY, 0);
	MsSpear.revol = app->physics->CreateRevoluteJoint(MsSpear.pbody, _body3, leftAnchorX, leftAnchorY , -0);

	//MrSpear.revolMe = app->physics->CreateRevoluteJoint( MrSpear.rotationB, MrSpear.pbody, 0, 0, 300);

	_detectionBody = app->physics->CreateRectangleSensor(position.x , position.y , 250, 200, bodyType::DYNAMIC, ColliderType::ENEMY);
	myBodies.Add(_detectionBody);
	_detectionBody->listener = this;
	_detectionBody->body->SetGravityScale(0);

	_body2->body->SetTransform(_body->body->GetPosition(), _body->body->GetAngle());
	_body3->body->SetTransform(_body->body->GetPosition(), _body->body->GetAngle());
	
	
	return true;
}

bool Aelfric::PreUpdate(float dt)
{

	return  true;
}

bool Aelfric::Update(float dt)
{
	
	MrSpear.revol->SetMotorSpeed(10);
	MsSpear.revol->SetMotorSpeed(-10);

	b2Vec2 Velocity;

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

	if (_body->active) {

		
		//MsSpear.pbody->body->SetAngularVelocity(20);
		

		_body->body->SetLinearVelocity(Velocity);
		_body2->body->SetLinearVelocity(Velocity);
		_body3->body->SetLinearVelocity(Velocity);
		currentAnimation->Update();

		// Blit

		position.x = METERS_TO_PIXELS(_body->body->GetTransform().p.x);
		position.y = METERS_TO_PIXELS(_body->body->GetTransform().p.y);

		//MrSpear.pbody->body->SetTransform(_body->body->GetPosition() + b2Vec2(1.0f, 0.0f), MrSpear.pbody->GetRotation());
		//MsSpear.pbody->body->SetTransform(_body->body->GetPosition() + b2Vec2(-1.0f, 0.0f), 0.0f);
		
		int R = 255, G = 255, B = 255;

		if (hurt == true) {
			if (hurtTimer.ReadMSec() < 800) {
				G = 0; B = 0;
			}
			else {
				hurt = false;
			}

		}
		

		int sp1Posx;
		int sp1Posy;
		MrSpear.pbody->GetPosition(sp1Posx, sp1Posy);
		int sp2Posx;
		int sp2Posy;
		MsSpear.pbody->GetPosition(sp2Posx, sp2Posy);
		
		//MrSpear.rotationB->body->SetTransform(MrSpear.pbody->body->GetPosition(), 0);
		_detectionBody->body->SetTransform(_body->body->GetPosition(),0.0f);
	//	MrSpear.rotationB->body->SetTransform(_body->body->GetPosition(), 0.0f);
	//	MsSpear.rotationB->body->SetTransform(_body->body->GetPosition(), 0.0f);

		SDL_Rect spearRect = { 560,1,17,85 };
		app->render->DrawTexture(texture, sp1Posx -8, sp1Posy-40, false, &spearRect, 255, 1,255,255,255, MrSpear.pbody->GetRotation());
		app->render->DrawTexture(texture, sp2Posx - 8, sp2Posy - 40, false, &spearRect, 255, 1, 255, 255, 255, MsSpear.pbody->GetRotation());
		if (myDir == Direction::LEFT) {
			app->render->DrawTexture(texture, position.x - 75, position.y - 50, true, &currentAnimation->GetCurrentFrame(), 255, 1, R, G, B);
		}
		else if (myDir == Direction::RIGHT) {
			app->render->DrawTexture(texture, position.x - 75, position.y - 50, false, &currentAnimation->GetCurrentFrame(), 255, 1, R, G, B);
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


		//MrSpear.revol->SetMotorSpeed(20);
		//MsSpear.revol->SetMotorSpeed(-20);
		MrSpear.pbody->body->SetAngularVelocity(-200);
		MsSpear.pbody->body->SetAngularVelocity(200);

		break;
	case ColliderType::ENEMY_ATTACK:
		LOG("erpt");
		HP--;
		hurt = true;
		hurtTimer.Start();
		break;
	case ColliderType::PLAYER_ATTACK:
		if (active) {
			LOG("erpt");
			HP--;
			hurt = true;
			hurtTimer.Start();
		}
		break;

	}
}

