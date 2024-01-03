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

	form1Anim.PushBack({ parameters.child("forms").child("form1").attribute("x").as_int(), parameters.child("forms").child("form1").attribute("y").as_int() ,parameters.child("forms").child("form1").attribute("h").as_int() ,parameters.child("forms").child("form1").attribute("w").as_int() });
	form3Anim.PushBack({parameters.child("forms").child("form3").attribute("x").as_int(), parameters.child("forms").child("form3").attribute("y").as_int(), parameters.child("forms").child("form3").attribute("h").as_int(), parameters.child("forms").child("form3").attribute("w").as_int()});
	return true;
}

bool Spear::Start() {

	//initilize textures

	texture = app->tex->Load(texturePath);

	b2Vec2 positiondissapera = b2Vec2(-120, -100);
	pbody = app->physics->CreateRectangle(positiondissapera.x + 16, positiondissapera.y + 16, 40, 16, bodyType::DYNAMIC, ColliderType::PLATFORM);
	pbody->ctype = ColliderType::SPEAR;
	pbody->listener = this;
	ThePlatform = app->physics->CreateRectangle(positiondissapera.x + 16, positiondissapera.y + 16, 40, 16, bodyType::STATIC, ColliderType::PLATFORM);
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
		isSticked = true;
		platform = false;

	}

	if (app->scene->player->power == PowerLvl::OP && spearUpgrade == false) 
	{
		b2Vec2 positiondissapera2 = b2Vec2(-240, -200);
		ThePlatform->body->SetTransform(positiondissapera2,0);
		b2Vec2 positiondissapera = b2Vec2(-120, -100);
		ThePlatform = app->physics->CreateRectangle(positiondissapera.x + 24, positiondissapera.y + 45, 60, 16, bodyType::STATIC, ColliderType::PLATFORM);
		ThePlatform->ctype = ColliderType::PLATFORM;
		spearUpgrade = true;
	}


	if (position.DistanceTo(app->scene->player->position) <= 5 && daPlatform == true /*app->scene->player->position.x - position.x == 0 && app->scene->player->position.y - position.y == 0 && daPlatform == true*/)
	{
 		isPicked = true;
		b2Vec2 positiondissapera = b2Vec2(-100, -100);
		pbody->body->SetTransform(positiondissapera, angle + 270);

		daPlatform = false;
	}
	if (app->scene->player->power != PowerLvl::OP)
	app->render->DrawTexture(texture, METERS_TO_PIXELS(ThePlatform->body->GetPosition().x-30), METERS_TO_PIXELS(ThePlatform->body->GetPosition().y -8), false,&form1Anim.GetCurrentFrame());
	else 
	app->render->DrawTexture(texture, METERS_TO_PIXELS(ThePlatform->body->GetPosition().x - 43), METERS_TO_PIXELS(ThePlatform->body->GetPosition().y - 8), false, &form3Anim.GetCurrentFrame());
	//app->render->DrawTexture(texture, METERS_TO_PIXELS(ThePlatformLong->body->GetPosition().x - 30), METERS_TO_PIXELS(ThePlatformLong->body->GetPosition().y - 8), false, &form3Anim.GetCurrentFrame());
	//app->render->DrawTexture(texture, position.x, position.y, false, 0,0,angle);

	return true;
}

bool Spear::CleanUp()
{
	return true;
}

void Spear::OnCollision(PhysBody* physA, PhysBody* physB) {


	switch (physB->ctype)
	{
	case ColliderType::ENEMY:
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


