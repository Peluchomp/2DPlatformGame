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

	

	return true;
}

bool Spear::Start() {

	//initilize textures
	

	return true;
}

bool Spear::Update(float dt)
{
	b2Vec2 vel;
	if (started == false) 
	{
		position.x = app->scene->player->position.x;
		position.y = app->scene->player->position.y;
		texturePath = "Assets/Textures/goldCoin.png";
		
		texture = app->tex->Load(texturePath);
		pbody = app->physics->CreateCircle(position.x + 16, position.y + 16, 16, bodyType::DYNAMIC);
		pbody->ctype = ColliderType::SPEAR;
		
		angle = app->scene->player->angle_deg;
		started = true;
		pbody->body->GetFixtureList()->SetSensor(true);
		SDL_GetMouseState(&x,&y);
		vel = b2Vec2(app->scene->player->delta_x, app->scene->player->delta_y);
		vel.Normalize();
		vel.x *=dt/2;
		vel.y *=dt/2;
		
		pbody->body->SetLinearVelocity(-vel);
	}
	
	pbody->body->SetGravityScale(0);
	pbody->body->SetTransform(pbody->body->GetPosition(), angle);

	
	

	// L07 DONE 4: Add a physics to an item - update the position of the object from the physics.  
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 16;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 16;

	app->render->DrawTexture(texture, position.x, position.y,0,0,angle + 270);

	return true;
}

bool Spear::CleanUp()
{
	return true;
}
