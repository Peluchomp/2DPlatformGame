#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "Map.h"
#include "Player.h"
#include "Physics.h"
#include "Defs.h"
#include "Log.h"

Scene::Scene() : Module()
{
	name.Create("scene");
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene");
	bool ret = true;
	app->map->name = config.child("map").attribute("name").as_string();
	app->map->path = config.child("map").attribute("path").as_string();

	scene_parameter = config;

	// iterate all objects in the scene
	// Check https://pugixml.org/docs/quickstart.html#access
	for (pugi::xml_node itemNode = config.child("item"); itemNode; itemNode = itemNode.next_sibling("item"))
	{
		Item* item = (Item*)app->entityManager->CreateEntity(EntityType::ITEM);
		item->parameters = itemNode;
	}

	for (pugi::xml_node itemNode = config.child("jorge"); itemNode; itemNode = itemNode.next_sibling("jorge"))
	{
		Jorge* jorge = (Jorge*)app->entityManager->CreateEntity(EntityType::JORGE);
		jorge->parameters = itemNode;
	}

	if (config.child("player")) {
		player = (Player*)app->entityManager->CreateEntity(EntityType::PLAYER);
		player->parameters = config.child("player");
	}

	const char* musicPath = config.child("level0_music").attribute("path").as_string();
	if (musicPath != nullptr) { app->audio->PlayMusic(musicPath); }

	if (config.child("spear")) {
		player->mySpear = (Spear*)app->entityManager->CreateEntity(EntityType::SPEAR);
		player->mySpear->parameters = config.child("spear");
	}

	//--------Spawn all Orbs----------//
	for (pugi::xml_node orbNode = config.child("orb_spawn"); orbNode; orbNode = orbNode.next_sibling("orb_spawn")) {
		Orb* orb = (Orb*)app->entityManager->CreateEntity(EntityType::ORB);
		orb->position.x = orbNode.attribute("x").as_int();
		orb->position.y = orbNode.attribute("y").as_int();
		orb->num = orbNode.attribute("num").as_int();
		orb->parameters = scene_parameter.child("orb");

		int p;
	
	}

	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	// NOTE: We have to avoid the use of paths in the code, we will move it later to a config file
	//img = app->tex->Load("Assets/Textures/test.png");

	//Music is commented so that you can add your own music
	//app->audio->PlayMusic("Assets/Audio/Music/music_spy.ogg");

	//Get the size of the window
	app->win->GetWindowSize(windowW, windowH);

	//Get the size of the texture
	app->tex->GetSize(img, texW, texH);

	textPosX = (float)windowW / 2 - (float)texW / 2;
	textPosY = (float)windowH / 2 - (float)texH / 2;

	//app->map->Load();

	SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d",
		app->map->mapData.width,
		app->map->mapData.height,
		app->map->mapData.tileWidth,
		app->map->mapData.tileHeight,
		app->map->mapData.tilesets.Count());

	mouseTileTex = app->tex->Load("Assets/Textures/goldCoin.png");

	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	float camSpeed = 1;
	if (app->input->GetKey(SDL_SCANCODE_P) == KEY_DOWN) {
		isInDebugMode = true;
	}
	if (app->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN) {
		isInDebugMode = false;
	}
	
		if (app->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
			app->render->camera.y -= (int)ceil(camSpeed * dt);

		if (app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
			app->render->camera.y += (int)ceil(camSpeed * dt);

		if (app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
			app->render->camera.x -= (int)ceil(camSpeed * dt);

		if (app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
			app->render->camera.x += (int)ceil(camSpeed * dt);


		if (isInDebugMode == false) {
			if (player->position.x < app->win->screenSurface->w / 3) {
				//player->position.x = app->win->screenSurface->w / 3;
			}
			//app->render->camera.x = -player->position.x * 2 - 3 + app->win->screenSurface->w / 2;

			if (player->position.y <= 0)
			{
				//player->position.y += app->win->screenSurface->h + 13 * 2;
			}
			else if (player->position.y >= app->win->screenSurface->h)
			{
				//player->position.y -= app->win->screenSurface->h - 13 * 2;
			}
			else
			{
				//player->position.y = 0;
			}
			//app->render->camera.y = -player->position.y +app->win->screenSurface->h/2-143;
		}
	
		app->render->camera.x = (-player->position.x) * app->win->GetScale() + 512;
		//app->render->camera.x = -app->scene->player->position.x + app->render->camera.w / 2;
		app->render->camera.y = -app->scene->player->position.y + app->render->camera.h / 2;
		if (app->render->camera.x > 0) app->render->camera.x = 0;
		if (app->render->camera.y > 0) app->render->camera.y = 0;
		/*if (app->render->camera.x < -app->map->mapData.width * app->map->mapData.tileWidth + app->render->camera.w)
			app->render->camera.x = (-player->position.x) * app->win->GetScale() + 512;*/
			if (app->render->camera.y < -app->map->mapData.height * app->map->mapData.tileHeight + app->render->camera.h)
				app->render->camera.y = (-app->map->mapData.height * app->map->mapData.tileHeight + app->render->camera.h) * 2;

/**2 - 3 + app->win->screenSurface->w / 2;*/
		


		app->render->camera.y = (-player->position.y) * app->win->GetScale() + 480;
	

		if (app->physics->debug && app->input->GetKey(SDL_SCANCODE_O) == KEY_DOWN) {
			for (int i = 0; i < 50; ++i) {
				if (orbs[i] == nullptr) {
					orbs[i] = (Orb*)app->entityManager->CreateEntity(EntityType::ORB);
					orbs[i]->parameters = scene_parameter.child("orb");
					orbs[i]->position.x = player->position.x; orbs[i]->position.y = 100;
					break;
				}
			}
			
		}

		
		// L14: TODO 3: Request App to Load / Save when pressing the keys F5 (save) / F6 (load)
		if (app->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN) app->SaveRequest();
		if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) app->LoadRequest();

	// Renders the image in the center of the screen 
	//app->render->DrawTexture(img, (int)textPosX, (int)textPosY);

	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;


	iPoint mousePos;
	app->input->GetMousePosition(mousePos.x, mousePos.y);
	iPoint mouseTile = app->map->WorldToMap(mousePos.x - app->render->camera.x,
		mousePos.y - app->render->camera.y);

	// Render a texture where the mouse is over to highlight the tile, use the texture 'mouseTileTex'
	iPoint highlightedTileWorld = app->map->MapToWorld(mouseTile.x, mouseTile.y);
	//app->render->DrawTexture(mouseTileTex, highlightedTileWorld.x, highlightedTileWorld.y, false);

	iPoint origin = iPoint(player->position.x, player->position.y);

	//If mouse button is pressed modify player position
	/*if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) {
		player->position = iPoint(highlightedTileWorld.x, highlightedTileWorld.y);
		app->map->pathfinding->CreatePath(origin, mouseTile);
	}*/

	// L13: Get the latest calculated path and draw
	const DynArray<iPoint>* path = app->map->pathfinding->GetLastPath();
	for (uint i = 0; i < path->Count(); ++i)
	{
		iPoint pos = app->map->MapToWorld(path->At(i)->x, path->At(i)->y);
		app->render->DrawTexture(mouseTileTex, pos.x, pos.y, false);
	}


	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}

bool Scene::LoadState(pugi::xml_node node) {

	player->position.x = node.child("position").attribute("x").as_int();
	player->position.y = node.child("position").attribute("y").as_int();

	player->pbody->SetPos(PIXEL_TO_METERS(node.child("position").attribute("x").as_int()), PIXEL_TO_METERS(node.child("position").attribute("y").as_int()));


	player->orbs = node.attribute("orbs").as_int();
	int level = node.attribute("PowerLvl").as_int();
	switch (level) {
	case(0):
		player->power = PowerLvl::NORMAL;
		break;
	case(1):
		player->power = PowerLvl::MID;
		break;
	case(2):
		player->power = PowerLvl::OP;
		break;
	}
	ListItem<Entity*>* item;

	bool ret = true;

	Entity* pEntity = NULL;

	// Spawn saved entities
	for (item = app->entityManager->savedEntities.start ; item != NULL && ret == true; item = item->next)
	{
		pEntity = item->data;
		
		if (pEntity->type == EntityType::ORB) {
			
			for (pugi::xml_node orbNode = scene_parameter.child("orb_spawn"); orbNode; orbNode = orbNode.next_sibling("orb_spawn")) {
				if (pEntity->num == orbNode.attribute("num").as_int()) {
					pEntity = app->entityManager->CreateEntity(EntityType::ORB);
					pEntity->position.x = orbNode.attribute("x").as_int();
					pEntity->position.y = orbNode.attribute("y").as_int();
					pEntity->num = orbNode.attribute("num").as_int();
					pEntity->parameters = scene_parameter.child("orb");

					int p;
				}
			}
			
		}

	}

	return true;
}

// L14: TODO 8: Create a method to save the state of the renderer
// using append_child and append_attribute
bool Scene::SaveState(pugi::xml_node node) {

	pugi::xml_node Node = node.append_child("position");
	Node.append_attribute("x").set_value(player->position.x);
	Node.append_attribute("y").set_value(player->position.y);

	node.append_attribute("orbs").set_value(player->orbs);
	node.append_attribute("PowerLvl").set_value(player->power);

	return true;
}