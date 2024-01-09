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
#include "../Chandelier.h"
#include "Log.h"
#include "../MegaMorgan.h"

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
	if (player == nullptr) {

		LOG("Loading Scene");
		bool ret = true;
		app->map->name = config.child("map").attribute("name").as_string();
		app->map->path = config.child("map").attribute("path").as_string();

		scene_parameter = config;

		// iterate all objects in the scene
		// Check https://pugixml.org/docs/quickstart.html#access



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

		}

		return ret;
	}
	else /*Level 2*/ {
		currentLvl++;
		LOG("Loading Scene");
		bool ret = true;
		app->map->name = config.child("map1").attribute("name").as_string();
		app->map->path = config.child("map1").attribute("path").as_string();

		scene_parameter = config;

		// iterate all objects in the scene
		// Check https://pugixml.org/docs/quickstart.html#access

		const char* musicPath = config.child("level0_music").attribute("path").as_string();
		if (musicPath != nullptr) { app->audio->PlayMusic(musicPath); }

		/*if (config.child("spear")) {
			player->mySpear = (Spear*)app->entityManager->CreateEntity(EntityType::SPEAR);
			player->mySpear->parameters = config.child("spear");
		}*/

		//--------Spawn all Orbs----------//
		for (pugi::xml_node orbNode = config.child("orb_spawn"); orbNode; orbNode = orbNode.next_sibling("orb_spawn")) {
			Orb* orb = (Orb*)app->entityManager->CreateEntity(EntityType::ORB);
			orb->position.x = orbNode.attribute("x").as_int();
			orb->position.y = orbNode.attribute("y").as_int();
			orb->num = orbNode.attribute("num").as_int();
			orb->parameters = scene_parameter.child("orb");

		}

		if (currentLvl == 1) {
			for (pugi::xml_node orbNode = config.child("chandelure"); orbNode; orbNode = orbNode.next_sibling("chandelure")) {
				Chandelier* orb = (Chandelier*)app->entityManager->CreateEntity(EntityType::CHANDELIER);
				orb->position.x = orbNode.attribute("x").as_int();
				orb->position.y = orbNode.attribute("y").as_int();
				orb->num = orbNode.attribute("num").as_int();
				orb->parameters = orbNode;
				orb->Awake();
				orb->Start();

			}
		}

		SpawnGoons();
		return ret;

	}
}

// Called before the first frame
bool Scene::Start()
{
	// NOTE: We have to avoid the use of paths in the code, we will move it later to a config file
	
	//Get the size of the window
	app->win->GetWindowSize(windowW, windowH);

	//Get the size of the texture
	app->tex->GetSize(img, texW, texH);

	textPosX = (float)windowW / 2 - (float)texW / 2;
	textPosY = (float)windowH / 2 - (float)texH / 2;

	enemyDeathEffect = app->audio->LoadFx(scene_parameter.child("enemyEffect").attribute("audiopath").as_string());

	SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d",
		app->map->mapData.width,
		app->map->mapData.height,
		app->map->mapData.tileWidth,
		app->map->mapData.tileHeight,
		app->map->mapData.tilesets.Count());


	

	const char* tilePath = scene_parameter.child("pathTile").attribute("texturepath").as_string();
	pathTexture = app->tex->Load(tilePath);

	SpawnGoons(true  );

	return true;
}

void Scene::SpawnGoons(bool first) {

	if (currentLevel == 0) {
		for (pugi::xml_node itemNode = scene_parameter.child("morgan"); itemNode; itemNode = itemNode.next_sibling("morgan"))
		{
			Morgan* item = (Morgan*)app->entityManager->CreateEntity(EntityType::MORGAN);
			item->parameters = itemNode;
			item->num = itemNode.attribute("num").as_int();
			item->position.x = itemNode.child("position").attribute("x").as_int() * 40;
			item->position.y = itemNode.child("position").attribute("y").as_int() * 40;
			if (!first)item->Start();
		}

		for (pugi::xml_node itemNode = scene_parameter.child("jorge"); itemNode; itemNode = itemNode.next_sibling("jorge"))
		{
			Jorge* jorge = (Jorge*)app->entityManager->CreateEntity(EntityType::JORGE);
			jorge->parameters = itemNode;
			jorge->num = itemNode.attribute("num").as_int();
			jorge->position.x = itemNode.child("position").attribute("x").as_int() * 40;
			jorge->position.y = itemNode.child("position").attribute("y").as_int() * 40;
			if (!first)jorge->Start();
		}
	}
	else {
		pugi::xml_node level1Node = scene_parameter.child("level1_spawns");

		for (pugi::xml_node itemNode = level1Node.child("morgan"); itemNode; itemNode = itemNode.next_sibling("morgan"))
		{
			Morgan* item = (Morgan*)app->entityManager->CreateEntity(EntityType::MORGAN);
			item->parameters = itemNode;
			item->num = itemNode.attribute("num").as_int();
			item->position.x = itemNode.child("position").attribute("x").as_int() * 40;
			item->position.y = itemNode.child("position").attribute("y").as_int() * 40;
			if (!first)item->Start();
		}

		for (pugi::xml_node itemNode = level1Node.child("mega_morgan"); itemNode; itemNode = itemNode.next_sibling("mega_morgan"))
		{
			MegaMorgan* item = (MegaMorgan*)app->entityManager->CreateEntity(EntityType::MEGA_MORGAN);
			item->parameters = itemNode;
			item->num = itemNode.attribute("num").as_int();
			item->position.x = itemNode.child("position").attribute("x").as_int() * 40;
			item->position.y = itemNode.child("position").attribute("y").as_int() * 40;
			if (!first)item->Start();
		}

		for (pugi::xml_node itemNode = level1Node.child("jorge"); itemNode; itemNode = itemNode.next_sibling("jorge"))
		{
			Jorge* jorge = (Jorge*)app->entityManager->CreateEntity(EntityType::JORGE);
			jorge->parameters = itemNode;
			jorge->num = itemNode.attribute("num").as_int();
			jorge->position.x = itemNode.child("position").attribute("x").as_int() * 40;
			jorge->position.y = itemNode.child("position").attribute("y").as_int() * 40;
			if (!first)jorge->Start();
		}

	}

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

	/*app->render->camera.x = (-player->position.x) * app->win->GetScale() + 512;
	if (app->render->camera.x > 0) app->render->camera.x = 0;
	if (app->render->camera.x < -app->map->mapData.width && app->map->mapData.tileWidth + app->render->camera.w)
		app->render->camera.x = (-player->position.x) * app->win->GetScale() + 512;
	app->render->camera.x = -app->scene->player->position.x + app->render->camera.w / 2;
	if (app->render->camera.y < 1000)
		app->render->camera.y = (-player->position.y) * app->win->GetScale() + 480;
*/


	

	if (currentLvl == 0) {
		
		app->render->camera.x = (-player->position.x) * app->win->GetScale() + 512;
		app->render->camera.x = -app->scene->player->position.x + app->render->camera.w / 2;
		if (app->render->camera.y < 1000)
			app->render->camera.y = (-player->position.y) * app->win->GetScale() + 480;


		app->render->camera.x = (-player->position.x) * app->win->GetScale() + 512;
		if (app->render->camera.x > 0) {
			app->render->camera.x = 0;
		}

			if (app->render->camera.y > 0) app->render->camera.y = 0;

				if (app->render->camera.y < -app->map->mapData.height * app->map->mapData.tileHeight + app->render->camera.h - 175)
					app->render->camera.y = (-app->map->mapData.height * app->map->mapData.tileHeight + app->render->camera.h - 175);

	}
	else if (currentLvl == 1)
	{
		if (app->render->camera.x > -680) {
			app->render->camera.x = (-player->position.x) * app->win->GetScale() + 512;

			app->render->camera.x = -app->scene->player->position.x + app->render->camera.w / 2;

			app->render->camera.x = (-player->position.x) * app->win->GetScale() + 512;
		}
		
		if (app->render->camera.y < 1000)
			app->render->camera.y = (-player->position.y) * app->win->GetScale() + 530;
		if (app->render->camera.x > 0) {
			app->render->camera.x = 0;
		}
	}

	/**2 - 3 + app->win->screenSurface->w / 2;*/



	//app->render->camera.y = (-player->position.y) * app->win->GetScale() + 480;


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
	

	iPoint origin = iPoint(player->position.x, player->position.y);


	// L13: Get the latest calculated path and draw
	const DynArray<iPoint>* path = app->map->pathfinding->GetLastPath();
	for (uint i = 0; i < path->Count(); ++i)
	{
		iPoint pos = app->map->MapToWorld(path->At(i)->x, path->At(i)->y);
		// app->render->DrawTexture(mouseTileTex, pos.x, pos.y, false);
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

	player->position.x = node.child(player->name.GetString()).child("position").attribute("x").as_int();
	player->position.y = node.child(player->name.GetString()).child("position").attribute("y").as_int();
	player->hp = node.child(player->name.GetString()).attribute("HP").as_int();
	player->pbody->SetPos(PIXEL_TO_METERS(node.child("player").child("position").attribute("x").as_int()), PIXEL_TO_METERS(node.child("player").child("position").attribute("y").as_int()));


	player->orbs = node.child("player").attribute("orbs").as_int();
	int level = node.child("player").attribute("PowerLvl").as_int();
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


	return true;
}

// L14: TODO 8: Create a method to save the state of the renderer
// using append_child and append_attribute
bool Scene::SaveState(pugi::xml_node node) {

	pugi::xml_node Node = node.append_child(player->name.GetString());

	Node.append_attribute("orbs").set_value(player->orbs);
	Node.append_attribute("PowerLvl").set_value(player->power);
	Node.append_attribute("HP").set_value(player->hp);
	Node = Node.append_child("position");
	Node.append_attribute("x").set_value(player->position.x);
	Node.append_attribute("y").set_value(player->position.y);





	return true;
}