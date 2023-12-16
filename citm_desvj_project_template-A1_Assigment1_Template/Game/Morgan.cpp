#include "Morgan.h"
#include "Source/App.h"
#include "Source/Textures.h"
#include "Source/Audio.h"
#include "Source/Input.h"
#include "Source/Render.h"
#include "Source/Scene.h"
#include "Source/Log.h"
#include "Source/Point.h"
#include "Source/Physics.h"

Morgan::Morgan() : Entity(EntityType::MORGAN)
{
	name.Create("morgan");
}

Morgan::~Morgan() {}

bool Morgan::Awake() {

	// the awake is only called for entities that are awaken with the manager



	return true;
}

bool Morgan::Start() {

	//initilize textures
	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();

	texture = app->tex->Load(texturePath);
	/*the pathTexture is given with the entity's creation*/
	pbody = app->physics->CreateCircle(position.x, position.y, 16, bodyType::DYNAMIC);
	pbody->ctype = ColliderType::ENEMY;
	pbody->body->SetGravityScale(1);

	LoadAnimations();


	return true;
}


bool Morgan::Update(float dt)
{
	if (pbody != nullptr) {

		// L07 DONE 4: Add a physics to an item - update the position of the object from the physics.  
		position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x);
		position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y);

		iPoint origin = iPoint(10, 136);
		iPoint destination = iPoint(20, 134);

		iPoint enemyPos = app->map->WorldToMap(position.x, position.y);
		iPoint playerPos = app->map->WorldToMap(app->scene->player->position.x, app->scene->player->position.y);

		app->map->pathfinding->CreatePath(enemyPos, playerPos);

		const DynArray<iPoint>* path = app->map->pathfinding->GetLastPath();

		for (uint i = 0; i < path->Count(); ++i)
		{
			iPoint pos = app->map->MapToWorld(path->At(i)->x, path->At(i)->y);
			app->render->DrawTexture(pathTexture, pos.x, pos.y, false);
		}

		if (path->Count() > 1 && app->map->pathfinding->CreatePath(enemyPos, playerPos) != -1) {
			iPoint pos = app->map->MapToWorld(path->At(1)->x, path->At(1)->y);


			if (enemyPos.x - playerPos.x < 0 && abs(enemyPos.x - playerPos.x) > 2)
				pbody->body->SetLinearVelocity(b2Vec2(1, 9.8f));
			else if (abs(enemyPos.x - playerPos.x) > 2)
				pbody->body->SetLinearVelocity(b2Vec2(-1, 9.8f));
			else if (abs(enemyPos.x - playerPos.x) < 2) {

				//aqui codigo de atacar
				pbody->body->SetLinearVelocity(b2Vec2(0, 9.8f));
				pbody->body->SetLinearDamping(0);
			}


			timer = 0;
		}

		if (app->map->pathfinding->CreatePath(enemyPos, playerPos) == -1) {

			pbody->body->SetLinearVelocity(b2Vec2(0, 9.8f));
			pbody->body->SetLinearDamping(0);
		}

		position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x);
		position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y);

		SDL_Rect section = { 1,1,99,78 };


		currentAnimation->Update();
		app->render->DrawTexture(texture, position.x - 64, position.y - 64, false, &currentAnimation->GetCurrentFrame());
		return true;
	}
}

bool Morgan::CleanUp()
{
	return true;
}

void Morgan::LoadAnimations() {

	for (pugi::xml_node node = parameters.child("animations").child("walking").child("frame"); node != NULL; node = node.next_sibling("frame")) {

		walking.PushBack({ node.attribute("x").as_int() , node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() });
		walking.speed = parameters.child("animations").child("walking").child("speed").attribute("value").as_float() / 16;
		//idle.loop = parameters.child("animations").child("idle").child("loop").attribute("value").as_bool();
	}

           walking.PushBack({ 1   ,1, 99 ,78 }) ;
		   walking.PushBack({ 101 ,1 ,99 ,78})	;
		   walking.PushBack({ 201 ,1 ,99 ,78})	;
		   walking.PushBack({ 301 ,1 ,99 ,78})	;
		   walking.PushBack({ 401 ,1 ,99 ,78})	;
		   walking.PushBack({ 501 ,1 ,99 ,78})	;
		   walking.PushBack({ 601 ,1 ,99 ,78})	;
		   walking.PushBack({ 701 ,1 ,99 ,78})	;
		   walking.PushBack({ 801 ,1 ,99 ,78})	;
		   walking.PushBack({ 901 ,1 ,99 ,78})	;
		   walking.PushBack({ 1001,1 ,99 ,78})	;
		   walking.PushBack({ 1101,1 ,99 ,78})	;
		   walking.speed = 0.1f / 16;

	currentAnimation = &walking;

}
