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

	_body = app->physics->CreateCircle(0, 0, 30, bodyType::DYNAMIC, ColliderType::ENEMY);
	_body->ctype = ColliderType::ENEMY;
	_body->body->SetGravityScale(1);

	myDir = Direction::RIGHT;
	ChangePosTimer.Start();

	MrSpear.texture = texture; MsSpear.texture = texture;
	MrSpear.pbody = app->physics->CreateRectangle(0, 0, 16, 85, bodyType::DYNAMIC, ColliderType::ENEMY_ATTACK);
	
	//MrSpear.dist = app->physics->CreateHorizontalDistanceJoint( MrSpear.pbody->body, _body->body, 2);

	return true;
}

bool Aelfric::PreUpdate(float dt)
{

	return  true;
}

bool Aelfric::Update(float dt)
{

	b2Vec2 Velocity;

	if (myDir == Direction::RIGHT) {

		Velocity.x = 1; Velocity.y = _body->body->GetLinearVelocity().y;
	}
	else if (myDir == Direction::LEFT) {
		Velocity.x = -1; Velocity.y = _body->body->GetLinearVelocity().y;

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




		_body->body->SetLinearVelocity(Velocity);
		currentAnimation->Update();

		// Blit

		position.x = METERS_TO_PIXELS(_body->body->GetTransform().p.x);
		position.y = METERS_TO_PIXELS(_body->body->GetTransform().p.y);

		int sp1Posx = METERS_TO_PIXELS(_body->body->GetTransform().p.x);
		int sp2Posx = METERS_TO_PIXELS(_body->body->GetTransform().p.x);

		SDL_Rect spearRect = { 560,1,17,85 };
		app->render->DrawTexture(texture, sp1Posx, sp2Posx, false, &spearRect);
		app->render->DrawTexture(texture, position.x -75, position.y-50, false, &currentAnimation->GetCurrentFrame());

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
		break;


	}
}

