#include "Spear.h"
#include "Source/App.h"
#include "Source/Textures.h"
#include "Source/Audio.h"
#include "Source/Input.h"
#include "Source/Render.h"
#include "Source/Scene.h"
#include "Source/Log.h"
#include "Source/Point.h"
#include "Source/Physics.h"

Spear::Spear() : Entity(EntityType::SPEAR)
{
	name.Create("spear");
}

Spear::~Spear() {}

bool Spear::Awake() {

	texturePath = parameters.attribute("texturepath").as_string();
	return true;
}

bool Spear::Start() {

	//initilize textures

	texture = app->tex->Load(texturePath);


	pbody = app->physics->CreateRectangle(position.x + 16, position.y + 16,40 , 16,bodyType::DYNAMIC);
	pbody->ctype = ColliderType::SPEAR;
	pbody->listener = this;
	ThePlatform = app->physics->CreateRectangle(position.x + 16, position.y + 16, 40, 16, bodyType::STATIC);
	ThePlatform->ctype = ColliderType::PLATFORM;
	return true;
}
bool Spear::PreUpdate(float dt) 
{
	
	return  true;
}

bool Spear::Update(float dt)
{
	
	b2Vec2 vel;

	pbody->body->SetTransform(pbody->body->GetPosition(), angle);
	if (started == false) 
	{
	
		pbody->body->SetTransform(app->scene->player->pbody->body->GetPosition(), angle+270);
		
		angle = app->scene->player->angle_deg;
		
	
		SDL_GetMouseState(&x,&y);
		vel = b2Vec2(app->scene->player->delta_x, app->scene->player->delta_y);
		vel.Normalize();
		vel.x *=dt/2;
		vel.y *=dt/2;
		
		pbody->body->SetLinearVelocity(-vel);
	}

	if (daPlatform == true) {
		pbody->body->GetFixtureList()->SetSensor(true);

		vel.x = app->scene->player->position.x-position.x;
		vel.y = app->scene->player->position.y - position.y;
		vel.Normalize();
		vel.x *= dt / 2;
		vel.y *= dt / 2;

		pbody->body->SetLinearVelocity(vel);
	}
	
	pbody->body->SetGravityScale(0);


	
	

	// L07 DONE 4: Add a physics to an item - update the position of the object from the physics.

	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 16;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 16;

	if (started == false) 
	{
		position.x = app->scene->player->position.x;
		position.y = app->scene->player->position.y;
		

	}

	if (started == false)
	{
		started = true;
		pbody->body->GetFixtureList()->SetSensor(true);
	}
	
	if (platform == true) 
	{
		b2Vec2 positiondissapera = b2Vec2(-100,-100);
		ThePlatform->body->SetTransform(pbody->body->GetPosition(),0);
		pbody->body->SetTransform(positiondissapera, angle + 270);
	
		platform = false;

	}



	app->render->DrawTexture(texture, position.x, position.y, false, 0,0,angle + 270);

	return true;
}

bool Spear::CleanUp()
{
	return true;
}

void Spear::OnCollision(PhysBody* physA, PhysBody* physB) {


	switch (physB->ctype)
	{
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		
		break;
	case ColliderType::PLATFORM:
		if (physA != ThePlatform && daPlatform == false || physB != ThePlatform && daPlatform == false) {
			pbody->body->GetFixtureList()->SetSensor(false);
			platform = true;

		}
		
		LOG("Collision PLATFORM");
		break;
	case ColliderType::PLAYER:
		pbody->body->GetFixtureList()->SetSensor(true);
		LOG("Collision SPEAR");
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	}
}


