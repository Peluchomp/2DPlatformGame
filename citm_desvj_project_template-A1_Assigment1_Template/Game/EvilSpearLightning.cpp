#include "EvilSpearLightning.h"
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

int getRandomNumber2(int min, int max) {
	// Seed the random number generator with the current time
	std::srand(static_cast<unsigned int>(std::time(0)));

	// Generate a random number between min and max (inclusive)
	return min + std::rand() % (max - min + 1);
}

EvilSpearLightning::EvilSpearLightning() : Entity(EntityType::EVILSPEARLIGHTNING)
{
	name.Create("evilspearlightning");
}

EvilSpearLightning::~EvilSpearLightning() {}

bool EvilSpearLightning::Awake() {

	// the awake is only called for entities that are awaken with the manager
	texture = app->tex->Load(parameters.child("texture").attribute("path").as_string());

	pbody = app->physics->CreateRectangle(position.x, position.y, 16, 200, bodyType::KINEMATIC, ColliderType::ENEMY);
	pbody->ctype = ColliderType::ENEMY;
	pbody->body->SetGravityScale(0);

	eviltwinpbody = app->physics->CreateRectangle(position.x, position.y, 16, 200, bodyType::KINEMATIC, ColliderType::ENEMY);
	eviltwinpbody->ctype = ColliderType::ENEMY;
	pbody->body->SetGravityScale(0);

	taimer.Start();

	myBodies.Add(pbody);

	return true;
}

bool EvilSpearLightning::Start() {

	//initilize textures



	return true;
}
bool EvilSpearLightning::PreUpdate(float dt)
{

	return  true;
}

bool EvilSpearLightning::Update(float dt)
{
	startAtackTimer++;

	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x);
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y);

	if (startAtackTimer > 300)
	{
		if (taimer.ReadSec() > 1) {

			if (getRandomNumber2(0, 1) == 0) {
				pbody->body->SetLinearVelocity({0.5f,0 });
			}
			else {
				pbody->body->SetLinearVelocity({ -0.5f,0 });
			}
		}
	}
	else
	{
		b2Vec2 pozition = { app->scene->player->pbody->body->GetPosition().x,app->scene->player->pbody->body->GetPosition().y - 1 };
		pbody->body->SetTransform(pozition, 0);
	}

	b2Vec2 pozition = { pbody->body->GetPosition().x + 2,pbody->body->GetPosition().y};
	eviltwinpbody->body->SetTransform(pozition, 0);

	app->render->DrawTexture(texture, pbody->body->GetPosition().x + 2, pbody->body->GetPosition().y, false);
	app->render->DrawTexture(texture, METERS_TO_PIXELS(eviltwinpbody->body->GetTransform().p.x), METERS_TO_PIXELS(eviltwinpbody->body->GetTransform().p.y), false);

	return true;
}

bool EvilSpearLightning::CleanUp()
{
	return true;
}

void EvilSpearLightning::OnCollision(PhysBody* physA, PhysBody* physB) {



}