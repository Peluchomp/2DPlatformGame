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

void EvilSpearLightning::SetSpeed(float speed) {
	this->speed = speed;
}

bool EvilSpearLightning::Awake() {

	// the awake is only called for entities that are awaken with the manager
	texture = app->tex->Load("Assets/Textures/priest.png");

	pbody = app->physics->CreateRectangle(position.x, position.y, 16, 350, bodyType::KINEMATIC, ColliderType::ENEMY);
	pbody->ctype = ColliderType::ENEMY;
	pbody->body->SetGravityScale(0);

	eviltwinpbody = app->physics->CreateRectangle(position.x, position.y, 16, 350, bodyType::KINEMATIC, ColliderType::ENEMY);
	eviltwinpbody->ctype = ColliderType::ENEMY;
	pbody->body->SetGravityScale(0);

	taimer.Start();

	myBodies.Add(pbody);
	myBodies.Add(eviltwinpbody);

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
	if (deathTaimer.ReadSec() > 3)
	{
		pendingToDestroy = true;
		Lthunder->pendingToDestroy = true;
		Rthunder->pendingToDestroy = true;
	}
 	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x);
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y);

	if (startAtackTimer > 150)
	{

		if (spawnLightning == true) {
			Lthunder = (Thunder*)app->entityManager->CreateEntity(EntityType::THUNDER);
			Lthunder->parameters = app->scene->scene_parameter.child("thunder");
			Lthunder->bossThunder = true;

			Rthunder = (Thunder*)app->entityManager->CreateEntity(EntityType::THUNDER);
			Rthunder->parameters = app->scene->scene_parameter.child("thunder");
			Rthunder->bossThunder = true;
			spawnLightning = false;
		}

		if (taimer.ReadSec() > 1) {

			if (getRandomNumber2(0, 1) == 0) {
				pbody->body->SetLinearVelocity({speed,0 });
			}
			else {
				pbody->body->SetLinearVelocity({ -speed,0 });
			}
		}
	}
	else
	{
		b2Vec2 pozition = { app->scene->player->pbody->body->GetPosition().x, 16.405 };
		pbody->body->SetTransform(pozition, 0);
	}
	if (spawnLightning == false) {
		Lthunder->bossPos.x = position.x - 65;
		Lthunder->bossPos.y = position.y - 120;
		Rthunder->bossPos.x = position.x + 40;
		Rthunder->bossPos.y = position.y - 120;
	}
	b2Vec2 pozition = { pbody->body->GetPosition().x + 2, 16.405};
	eviltwinpbody->body->SetTransform(pozition, 0);
	SDL_Rect spearRect = { 34,4,15,81 };
	app->render->DrawTexture(texture, METERS_TO_PIXELS(pbody->body->GetTransform().p.x), METERS_TO_PIXELS(pbody->body->GetTransform().p.y - 130), false, &spearRect);
	app->render->DrawTexture(texture, METERS_TO_PIXELS(eviltwinpbody->body->GetTransform().p.x), METERS_TO_PIXELS(eviltwinpbody->body->GetTransform().p.y - 130), false, &spearRect);

	return true;
}

bool EvilSpearLightning::CleanUp()
{
	return true;
}

void EvilSpearLightning::OnCollision(PhysBody* physA, PhysBody* physB) {



}