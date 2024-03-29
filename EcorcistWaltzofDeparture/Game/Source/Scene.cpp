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
#include "../Checkpoint.h"
#include "../Aelfric.h"
#include "../EvilSpin.h"
#include "SDL_mixer/include/SDL_mixer.h"
#include "../GuiSlider.h"
#include "../TitleScreen.h"
#include "GuiControl.h"
#include "GuiManager.h"
#include <string> 

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
	if (active) {



		if (currentLvl == 0) {
			levelTimer.Start();

			if (blackDetection != nullptr)
			{
				blackDetection->active = false;
			}


		    LOG("Loading Scene");
			bool ret = true;
			app->map->name = config.child("map").attribute("name").as_string();
			app->map->path = config.child("map").attribute("path").as_string();

			scene_parameter = config;
			if (itemTexture == nullptr) { itemTexture = app->tex->Load(config.child("items").attribute("texturePath").as_string()); }

			// iterate all objects in the scene
			// Check https://pugixml.org/docs/quickstart.html#access

			if (player == nullptr) {

				if (config.child("player")) {
					player = (Player*)app->entityManager->CreateEntity(EntityType::PLAYER);
					player->parameters = config.child("player");
					player->Awake();

				}
			}
			

			const char* musicPath = config.child("level0_music").attribute("path").as_string();
			if (musicPath != nullptr) { app->audio->PlayMusic(musicPath); }
			bossMusicPath = config.child("boss_music").attribute("path").as_string();


			if (player->mySpear == nullptr) {
				if (config.child("spear")) {
					player->mySpear = (Spear*)app->entityManager->CreateEntity(EntityType::SPEAR);
					player->mySpear->parameters = config.child("spear");
					player->mySpear->Awake();
					player->Start();
					player->mySpear->Start();
				}
			}
			//--------Spawn all Orbs----------//
			for (pugi::xml_node orbNode = config.child("orb_spawn"); orbNode; orbNode = orbNode.next_sibling("orb_spawn")) {
				Orb* orb = (Orb*)app->entityManager->CreateEntity(EntityType::ORB);
				orb->position.x = orbNode.attribute("x").as_int();
				orb->position.y = orbNode.attribute("y").as_int();
				orb->num = orbNode.attribute("num").as_int();
				orb->parameters = scene_parameter.child("orb");

			}

			for (pugi::xml_node checkpointNode = config.child("checkpoint_spawn"); checkpointNode; checkpointNode = checkpointNode.next_sibling("checkpoint_spawn")) {
				Checkpoint* checkpoint = (Checkpoint*)app->entityManager->CreateEntity(EntityType::CHECKPOINT);
				checkpoint->position.x = checkpointNode.attribute("x").as_int();
				checkpoint->position.y = checkpointNode.attribute("y").as_int();
				checkpoint->num = checkpointNode.attribute("num").as_int();
				checkpoint->parameters = scene_parameter.child("checkpoint");

			}

			return ret;
		}
		else /*Level 2*/ {
			levelTimer.Start();
			bool ret = true;
			app->map->name = config.child("map1").attribute("name").as_string();
			app->map->path = config.child("map1").attribute("path").as_string();

			scene_parameter = config;
			app->map->firstLoad = false;

			// iterate all objects in the scene
			// Check https://pugixml.org/docs/quickstart.html#access

			const char* musicPath = config.child("level1_music").attribute("path").as_string();
			if (musicPath != nullptr) { app->audio->PlayMusic(musicPath,-1); }

			/*if (config.child("spear")) {
				player->mySpear = (Spear*)app->entityManager->CreateEntity(EntityType::SPEAR);
				player->mySpear->parameters = config.child("spear");
			}*/

			//--------Spawn all Orbs----------//
			for (pugi::xml_node orbNode = config.child("orb_spawn1"); orbNode; orbNode = orbNode.next_sibling("orb_spawn1")) {
				Orb* orb = (Orb*)app->entityManager->CreateEntity(EntityType::ORB);
				orb->position.x = orbNode.attribute("x").as_int();
				orb->position.y = orbNode.attribute("y").as_int();
				orb->num = orbNode.attribute("num").as_int();
				orb->parameters = scene_parameter.child("orb");

			}

			chandelierDeathFx = app->audio->LoadFx(config.child("chandelierFx").attribute("audiopath").as_string());
			if (currentLvl == 1) {
				for (pugi::xml_node orbNode = config.child("chandelure"); orbNode; orbNode = orbNode.next_sibling("chandelure")) {
					Chandelier* orb = (Chandelier*)app->entityManager->CreateEntity(EntityType::CHANDELIER);
					orb->position.x = orbNode.attribute("x").as_int();
					orb->position.y = orbNode.attribute("y").as_int();
					orb->num = orbNode.attribute("num").as_int();
					orb->parameters = orbNode;
					orb->Awake();
					orb->Start();
					orb->breackFx = chandelierDeathFx;
				}
			}

			if (father == nullptr) {
				father = (Aelfric*)app->entityManager->CreateEntity(EntityType::AELFRIC);
				father->parameters = config.child("aelfric");
				father->Start();
			}

			blackDetection = app->physics->CreateRectangleSensor(15 * 40, 5 * 40, 200, 200, bodyType::STATIC, ColliderType::PLATFORM);
			blackDetection->listener = player;
			blackDetection->ctype = ColliderType::BLACK_TRIGGER;

			SpawnGoons();
			return ret;

		}
	}
	return true;
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
	bossGreetingFx = app->audio->LoadFx(scene_parameter.child("bossGreetingFx").attribute("audiopath").as_string());

	SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d",
		app->map->mapData.width,
		app->map->mapData.height,
		app->map->mapData.tileWidth,
		app->map->mapData.tileHeight,
		app->map->mapData.tilesets.Count());


	SDL_Rect btPos = { windowW / 2 - 60, windowH / 2 + 200, 120,20 };
	SDL_Rect resumePos = { windowW / 2 - 60, windowH / 2 , 120,20 };
	SDL_Rect backPos = { windowW / 2 - 60, windowH / 2 + 25, 120,20 };
	gcButtom = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, "   Exit   ", btPos, this);
	backtittleButtom = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 43, "Back To tittle", backPos, this);
	resumeButtom = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 5, "   Resume   ", resumePos, this);
	musicButtom = app->titleS->musicButtom; //(GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::SLIDER, 2, "   Sound   ", musicbtPos, this);
	fullScreenButtom = app->titleS->fullScreenButtom;//(GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::CHECKBOX, 3, "   Fullscreen   ", fullscreenbtPos, this);
	vSyncButtom = app->titleS->vSyncButtom;//(GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::CHECKBOX, 4, "   Vsync   ", vSyncPos, this);
	ReviveButtom = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 50, "   Try again   ", resumePos, this);
	const char* tilePath = scene_parameter.child("pathTile").attribute("texturepath").as_string();
	pathTexture = app->tex->Load(tilePath);

	

	SpawnGoons(true);

	return true;
}

void Scene::SpawnGoons(bool first) {

	if (currentLvl == 0) {
		for (pugi::xml_node itemNode = scene_parameter.child("morgan"); itemNode; itemNode = itemNode.next_sibling("morgan"))
		{
			Morgan* item = (Morgan*)app->entityManager->CreateEntity(EntityType::MORGAN);
			item->parameters = itemNode;
			item->num = itemNode.attribute("num").as_int();
			item->position.x = itemNode.child("position").attribute("x").as_int() * 40;
			item->position.y = itemNode.child("position").attribute("y").as_int() * 40;
			item->Start();
		}

		for (pugi::xml_node itemNode = scene_parameter.child("jorge"); itemNode; itemNode = itemNode.next_sibling("jorge"))
		{
			Jorge* jorge = (Jorge*)app->entityManager->CreateEntity(EntityType::JORGE);
			jorge->parameters = itemNode;
			jorge->num = itemNode.attribute("num").as_int();
			jorge->position.x = itemNode.child("position").attribute("x").as_int() * 40;
			jorge->position.y = itemNode.child("position").attribute("y").as_int() * 40;
			jorge->Start();
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
			item->Start();
		}

		for (pugi::xml_node itemNode = level1Node.child("mega_morgan"); itemNode; itemNode = itemNode.next_sibling("mega_morgan"))
		{
			MegaMorgan* item = (MegaMorgan*)app->entityManager->CreateEntity(EntityType::MEGA_MORGAN);
			item->parameters = itemNode;
			item->num = itemNode.attribute("num").as_int();
			item->position.x = itemNode.child("position").attribute("x").as_int() * 40;
			item->position.y = itemNode.child("position").attribute("y").as_int() * 40;
			item->Start();
		}

		for (pugi::xml_node itemNode = level1Node.child("jorge"); itemNode; itemNode = itemNode.next_sibling("jorge"))
		{
			Jorge* jorge = (Jorge*)app->entityManager->CreateEntity(EntityType::JORGE);
			jorge->parameters = itemNode;
			jorge->num = itemNode.attribute("num").as_int();
			jorge->position.x = itemNode.child("position").attribute("x").as_int() * 40;
			jorge->position.y = itemNode.child("position").attribute("y").as_int() * 40;
			jorge->Start();
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

	

	
	// Camera behaviour, depends on the level
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

		app->render->camera.x = (-player->position.x) * app->win->GetScale() + 512;

		app->render->camera.x = -app->scene->player->position.x + app->render->camera.w / 2;

		app->render->camera.x = (-player->position.x) * app->win->GetScale() + 512;

		if (!noir && app->render->camera.x < -680 && !bossZone) {
			app->render->camera.x = -680;
		}
		
	
		
		if (app->render->camera.y < -100)
			app->render->camera.y = (-player->position.y) * app->win->GetScale() + 530;
		else if (app->scene->player->position.y > 279)
			app->render->camera.y = (-player->position.y) * app->win->GetScale() + 530;

		if (app->render->camera.x > 0) {
			app->render->camera.x = 0;
		}
		if (player->position.x > 4240 && noir) {
			// Lock camera position to boss room
			app->render->camera.x = -8480; app->render->camera.y = -1290;
			bossDoor =  app->physics->CreateRectangle(106 * 40, 22 * 40, 42, 160, bodyType::STATIC, ColliderType::PLATFORM);
			bossDoor->ctype = ColliderType::PLATFORM;
			bossZone = true;
			tietleTimer.Start();
			app->audio->PlayFx(bossGreetingFx);
			noir = false;

		}
		if (bossZone) {
			app->render->camera.x = -8480; app->render->camera.y = -1290;
		}
	}

	// Debug spawn orbs
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

	// Debug spawn rotating spears
	if (app->input->GetKey(SDL_SCANCODE_B) == KEY_DOWN) {

		EvilSpin* ev = (EvilSpin*)app->entityManager->CreateEntity(EntityType::EVILSPIN);
		ev->position.x = player->position.x; ev->position.y = player->position.y - 100;
		ev->Awake();

	}


	// L14: TODO 3: Request App to Load / Save when pressing the keys F5 (save) / F6 (load)
	if (app->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN) app->SaveRequest();
	if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) app->LoadRequest();


	if (app->scene->player->deathScreen == true)
	{
		SDL_Rect squarePos = { app->scene->player->position.x, app->scene->player->position.y, 1200,1200 };
		app->titleS->options = true;
		ReviveButtom->state = GuiControlState::NORMAL;
		app->render->DrawRectangle(squarePos, 0, 0, 0, 255, true, false);

	}
	else
	{
		ReviveButtom->state = GuiControlState::DISABLED;
	}

	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if (exit == true)
		ret = false;


	iPoint mousePos;
	app->input->GetMousePosition(mousePos.x, mousePos.y);
	iPoint mouseTile = app->map->WorldToMap(mousePos.x - app->render->camera.x,
		mousePos.y - app->render->camera.y);

	if (bossZone && tietleTimer.ReadSec() < 4) {
		titleAlpha = titleAlpha + (0.01) * (255 - titleAlpha);
			
		app->render->DrawTexture(titleCardTex,- app->render->camera.x/2,- app->render->camera.y/2,false, &titleCard, titleAlpha);

	}


	levelTimeDisplay = "Time:" + std::to_string(levelTimer.ReadSec());
	app->render->DrawText(levelTimeDisplay.c_str(), app->scene->windowW / 2 + 420 , app->scene->windowH / 2 - 380, 80, 40);

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

	//----------------Score stuf----------------//

	//volume sounds
	Mix_VolumeMusic(volume);
	Mix_Volume(-1, volume);
	
	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}

bool Scene::LoadState(pugi::xml_node node) {

	int actualLevel = node.attribute("currentLevel").as_int();

	if (currentLvl != actualLevel) {
		LOG("Have to recreate level");
		noir = false;
		prevLevel = actualLevel;
		currentLvl = actualLevel;

		if (actualLevel == 0) { blackDetection->active = false; }

		app->physics->DestroyPlatforms();
		app->entityManager->DestroyAll();
		

		app->map->CleanUp();
		app->scene->Awake(app->scene->scene_parameter);
		app->map->mapData.layers.Clear();
		app->map->Start();

		
	}


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

	node.append_attribute("currentLevel").set_value(currentLvl);

	pugi::xml_node Node = node.append_child(player->name.GetString());

	Node.append_attribute("orbs").set_value(player->orbs);
	Node.append_attribute("PowerLvl").set_value(player->power);
	Node.append_attribute("HP").set_value(player->hp);
	Node = Node.append_child("position");
	Node.append_attribute("x").set_value(player->position.x);
	Node.append_attribute("y").set_value(player->position.y);





	return true;
}

void Scene::checkAndRecreateLvl() {

	if (currentLvl != prevLevel) {
		LOG("Have to recreate level");
		noir = false;
		prevLevel = currentLvl;
		currentLvl = prevLevel;

		if (currentLvl == 0) { blackDetection->active = false; }

		app->physics->DestroyPlatforms();
		app->entityManager->DestroyAll();


		app->map->CleanUp();
		app->scene->Awake(app->scene->scene_parameter);
		app->map->mapData.layers.Clear();
		app->map->Start();


	}
}

bool Scene::OnGuiMouseClickEvent(GuiControl* control)
{
	fullscreenOnce++;
	vsyncOnce++;
	// L15: DONE 5: Implement the OnGuiMouseClickEvent method
	if (control->id == 1) {
		exit = true;
	}
	if (control->id == 3 && fullscreen == false && fullscreenOnce > 1) {
		fullscreen = true;

		fullscreenOnce = 0;
		SDL_SetWindowFullscreen(app->win->window, SDL_WINDOW_FULLSCREEN);
	}
	if (control->id == 5) {
		app->titleS->options = false;
	}

	if (control->id == 43) {
		app->titleS->back = true;
	}

	if (control->id == 50) {
		app->scene->player->deathScreen = false;
	    app->scene->player->Spawn(app->scene->currentLvl,true);

	}
	
	if (control->id == 3 && fullscreen == true && fullscreenOnce > 1) {
		

		Uint32 flags = SDL_WINDOW_SHOWN;
		flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

		SDL_SetWindowFullscreen(app->win->window, flags);

		SDL_RenderSetLogicalSize(app->render->renderer, app->win->width, app->win->height);

		app->win->screenSurface= SDL_GetWindowSurface(app->win->window);

	}
	
	if (control->id == 3 && fullscreen == true && fullscreenOnce > 1) {
		

		fullscreen = false;
		fullscreenOnce = 0;
		uint heigth;
		uint width;

		app->win->GetWindowSize(width, heigth);
		
		SDL_SetWindowFullscreen(app->win->window, 0);
		SDL_SetWindowSize(app->win->window, width,heigth );
		

	}

	if (control->id == 12 && vSync == false && vsyncOnce > 1) {
		vSync = true;
		vsyncOnce = 0;
		
	}
	else if (control->id == 12 && vSync == true && vsyncOnce > 1) {
		vSync = false;
		vsyncOnce = 0;
	}
	

	return true;
}
