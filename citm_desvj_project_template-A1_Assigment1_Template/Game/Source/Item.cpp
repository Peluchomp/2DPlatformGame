#include "Item.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"
#include "Window.h"
#include "Player.h"
Item::Item() : Entity(EntityType::ITEM)
{
	name.Create("item");
}

Item::~Item() {}

bool Item::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();

	return true;
}

bool Item::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);
	texture2 = app->tex->Load("Assets/Textures/Morgan.png");
	pbody = app->physics->CreateCircle(position.x, position.y, 16, bodyType::DYNAMIC);
	pbody->ctype = ColliderType::ITEM;
	pbody->body->SetGravityScale(0);
	

	return true;
}

bool Item::Update(float dt)
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

	for (uint i = 0; i < path->Count(); ++i)
	{
		iPoint pos = app->map->MapToWorld(path->At(i)->x, path->At(i)->y);
		app->render->DrawTexture(texture, pos.x, pos.y,false);
	}

	if (timer>=60 && path->Count()>2) {
		iPoint pos = app->map->MapToWorld(path->At(2)->x, path->At(2)->y);
		
		pbody->body->SetTransform(b2Vec2(PIXEL_TO_METERS(pos.x), PIXEL_TO_METERS(pos.y)), 0);
		timer = 0;
	}
	timer++;

	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x);
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y);

	SDL_Rect section = { 1,1,99,78 };
	app->render->DrawTexture(texture2, position.x - 64, position.y - 64, false, &section);

	//iPoint highlightedTileWorld = app->scene->player->position;
	//app->render->DrawTexture(texture, highlightedTileWorld.x, highlightedTileWorld.y, false);

	//iPoint origin = app->map->WorldToMap(app->scene->player->position.x, app->scene->player->position.y+10);
	//iPoint origin2 = app->map->WorldToMap(position.x,position.y);

	//
	////If mouse button is pressed modify player position
	//
	//	
	//	app->map->pathfinding->CreatePath(origin, origin2);
	//
	//	iPoint origin3;
	//// L13: Get the latest calculated path and draw
	//const DynArray<iPoint>* path = app->map->pathfinding->GetLastPath();
	//for (uint i = 0; i < path->Count(); ++i)
	//{

	//	iPoint pos = app->map->MapToWorld(path->At(i)->x, path->At(i)->y);
	//	app->render->DrawTexture(texture, pos.x, pos.y, false);
	//	
	//}




	//if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) {
	//	origin3 = app->map->MapToWorld(path->At(0)->x,path->At(0)->y);
	//	pbody->body->SetTransform(b2Vec2(PIXEL_TO_METERS(origin.x),PIXEL_TO_METERS(origin.y)),0);
	//}
	//
	//
	//
	//app->render->DrawTexture(texture, position.x, position.y, false);

	return true;
}

bool Item::CleanUp()
{
	return true;
}