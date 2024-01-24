#include "FloorSpears.h"
#include "Source/App.h"
#include "Source/Textures.h"
#include "Source/Audio.h"
#include "Source/Input.h"
#include "Source/Render.h"
#include "Source/Scene.h"
#include "Source/Log.h"
#include "Source/Point.h"
#include "Source/Physics.h"
#include "Source/Player.h"
#include <cstdlib>
#include <ctime>

int getRandomNumber5(int min, int max) {
	// Seed the random number generator with the current time
	std::srand(static_cast<unsigned int>(std::time(0)));

	// Generate a random number between min and max (inclusive)
	return min + std::rand() % (max - min + 1);
}

FloorSpears::FloorSpears() : Entity(EntityType::EVILSPEARLIGHTNING)
{
	name.Create("evilspearlightning");
}

FloorSpears::~FloorSpears() {}

bool FloorSpears::Awake() {

	// the awake is only called for entities that are awaken with the manager
	texture = app->tex->Load("Assets/Textures/priest2.png");

	pbody = app->physics->CreateRectangle(position.x, position.y, 10, 40, bodyType::KINEMATIC, ColliderType::ENEMY);
	pbody->ctype = ColliderType::ENEMY;
	pbody->body->SetGravityScale(0);

	
	pbody->body->SetGravityScale(0);

	taimer.Start();

	myBodies.Add(pbody);

	return true;
}

bool FloorSpears::Start() {

	//initilize textures



	return true;
}
bool FloorSpears::PreUpdate(float dt)
{

	return  true;
}

bool FloorSpears::Update(float dt)
{
	startAtackTimer++;

	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x);
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y);

	if (startAtackTimer > 60)
	{
		pbody->body->SetLinearVelocity({ 0, -2 });
		
	}
	else if (spawn == false)
	{
		b2Vec2 pozition;

		if (getRandomNumber5(0,1) == 0) {
				if (app->scene->player->myDir == Direction::LEFT)
				pozition = { app->scene->player->pbody->body->GetPosition().x + 0.25f,app->scene->player->pbody->body->GetPosition().y + 2};
			if (app->scene->player->myDir == Direction::RIGHT)
				pozition = { app->scene->player->pbody->body->GetPosition().x + 1.5f,app->scene->player->pbody->body->GetPosition().y + 2 };
			pbody->body->SetTransform(pozition, 0);
		}
		else {
			pozition = { app->scene->player->pbody->body->GetPosition().x + 0.75f,app->scene->player->pbody->body->GetPosition().y + 2 };
			pbody->body->SetTransform(pozition, 0);
		}
	
		spawn = true;
	}

	SDL_Rect spearRect = { 19,8,15,81 };
	app->render->DrawTexture(texture, METERS_TO_PIXELS(pbody->body->GetTransform().p.x-6), METERS_TO_PIXELS(pbody->body->GetTransform().p.y-45), false, &spearRect);

	return true;
}

bool FloorSpears::CleanUp()
{
	return true;
}

void FloorSpears::OnCollision(PhysBody* physA, PhysBody* physB) {



}