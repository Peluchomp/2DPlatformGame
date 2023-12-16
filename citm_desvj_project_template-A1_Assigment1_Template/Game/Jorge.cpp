#include "Jorge.h"
#include "../App.h"
#include "../Textures.h"
#include "../Audio.h"
#include "../Input.h"
#include "../Render.h"
#include "../Scene.h"
#include "../Log.h"
#include "../Point.h"
#include "../Physics.h"
#include "../Window.h"
#include "../Player.h"
Jorge::Jorge() : Entity(EntityType::JORGE)
{
	name.Create("jorge");
}

Jorge::~Jorge() {}

bool Jorge::Awake() {

	

	return true;
}

bool Jorge::Start() {

	//initilize textures
	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();

	texture = app->tex->Load(texturePath);
	texture2 = app->tex->Load("Assets/Textures/goldCoin.png");
	pbody = app->physics->CreateCircle(position.x, position.y, 16, bodyType::DYNAMIC);
	pbody->ctype = ColliderType::ENEMY;
	pbody->body->SetGravityScale(0);



	return true;
}

bool Jorge::Update(float dt)
{
	// L07 DONE 4: Add a physics to an item - update the position of the object from the physics.  
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x);
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y);

	iPoint origin = iPoint(10, 136);
	iPoint destination = iPoint(20, 134);

	iPoint enemyPos = app->map->WorldToMap(position.x, position.y);
	iPoint playerPos = app->map->WorldToMap(app->scene->player->position.x, app->scene->player->position.y);

	app->map->pathfinding->CreatePath(enemyPos, playerPos);

	const DynArray<iPoint>* path = app->map->pathfinding->GetLastPath();
	b2Vec2 posi;
	for (uint i = 0; i < path->Count(); ++i)
	{
		iPoint pos = app->map->MapToWorld(path->At(i)->x, path->At(i)->y);
		app->render->DrawTexture(texture, pos.x, pos.y, false);
		
	}

	if (path->Count() > 2 && app->map->pathfinding->CreatePath(enemyPos, playerPos) != -1) {

		iPoint pos = app->map->MapToWorld(path->At(2)->x, path->At(2)->y);
	
		float dirx = position.x - pos.x;
		float diry = position.y - pos.y;

		pbody->body->SetLinearVelocity(b2Vec2(-dirx/30,-diry/30));

	 if (abs(enemyPos.x - playerPos.x) < 2) {

		//aqui codigo de atacar
		pbody->body->SetLinearVelocity(b2Vec2(0, 0));
		pbody->body->SetLinearDamping(0);
	}

		
	}



	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x);
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y);

	SDL_Rect section = { 1,1,99,78 };
	app->render->DrawTexture(texture2, position.x - 64, position.y - 64, false, &section);



	return true;
}

bool Jorge::CleanUp()
{
	return true;
}