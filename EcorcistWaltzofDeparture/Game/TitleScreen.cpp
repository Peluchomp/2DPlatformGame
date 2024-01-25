#include "../App.h"
#include "../Input.h"
#include "../Textures.h"
#include "../Audio.h"
#include "../Render.h"
#include "../Window.h"
#include "../Scene.h"
#include "../Map.h"
#include "../Player.h"
#include "../Physics.h"
#include "../Defs.h"
#include "Chandelier.h"
#include "../Log.h"
#include "MegaMorgan.h"
#include "Checkpoint.h"
#include "../GuiControl.h"
#include "../GuiManager.h"
#include "GuiSlider.h"
#include "TitleScreen.h"
#include <string>
#include "SDL_mixer/include/SDL_mixer.h"

TitleScreen::TitleScreen() : Module()
{
	name.Create("title_screen");
}

// Destructor
TitleScreen::~TitleScreen()
{}

// Called before render is available
bool TitleScreen::Awake(pugi::xml_node& config)
{

	app->audio->PlayMusic(config.child("music").attribute("path").as_string(), 0);

	mynode = config;
	return true;
}

// Called before the first frame
bool TitleScreen::Start()
{
	pugi::xml_node config = mynode;
	if (active) {
		float timeToSpawn = 0;
		for (pugi::xml_node pNode = config.child("page"); pNode; pNode = pNode.next_sibling("page")) {
			for (pugi::xml_node fNode = pNode.child("frame"); fNode; fNode = fNode.next_sibling("frame")) {
				SDL_Rect rect = { fNode.attribute("x").as_int(), fNode.attribute("y").as_int() ,fNode.attribute("w").as_int() ,fNode.attribute("h").as_int() };
				SString apo = fNode.attribute("appearance").as_string();
				Appearance aporanc;
				if (apo == "right") { aporanc = Appearance::RIGHTWARDS; }
				else if (apo == "left") { aporanc = Appearance::LEFTWARDS; }
				else if (apo == "down") { aporanc = Appearance::DOWN; }
				else if (apo == "up") { aporanc = Appearance::UP; }
				else if (apo == "fade") { aporanc = Appearance::FADE; }

				float interpolation = fNode.attribute("interpolation").as_float();
				if (interpolation == 0) { interpolation = 0.005f; }

				Frame* f = new Frame(iPoint(fNode.attribute("desiredX").as_int(), fNode.attribute("desiredY").as_int()), interpolation , aporanc, rect, fNode.attribute("timeStay").as_float()*1000, fNode.attribute("timeStart").as_float() * 1000);
				f->texture = app->tex->Load(pNode.attribute("texturepath").as_string());
				myFrames.Add(f);
			}



		}

	}
	finalFrame = app->tex->Load(config.child("finalframe").attribute("texturepath").as_string());
	Logo.texture = app->tex->Load(config.child("logo").attribute("texturepath").as_string());

	Flame = new Icon();
	Flame->texture = app->tex->Load(config.child("flame").attribute("texturepath").as_string());
	for (pugi::xml_node pNode = config.child("flame").child("frame"); pNode; pNode = pNode.next_sibling("frame")) {
		Flame->defaultAnim.PushBack(SDL_Rect{ pNode.attribute("x").as_int(), pNode.attribute("y").as_int(),pNode.attribute("w").as_int(),pNode.attribute("h").as_int() });
	}
	Flame->defaultAnim.speed = 0.2f / 16.0f;
	Flame->currentAnim = &Flame->defaultAnim;
	spearFrame =  new Frame(iPoint(-30, 70), 0.005f, Appearance::LEFTWARDS, SDL_Rect{ 0,0,640,185 }, 10000, 45200);
	spearFrame->texture = app->tex->Load(config.child("spear").attribute("texturepath").as_string());

	delayTimer.Start();

	app->win->GetWindowSize(windowW, windowH);

	skip = false;
	stopesquizo = 0;
	startt = false;

	SDL_Rect btPos = { windowW / 2 - 60, windowH / 2 + 50, 120,20 };
	SDL_Rect exitPos = { windowW / 2 - 60, windowH / 2 + 250, 120,20 };
	SDL_Rect optionsPos = { windowW / 2 - 60, windowH / 2+150 , 120,20 };
	SDL_Rect musicbtPos = { windowW / 2 - 60, windowH / 2 + 120 , 120,20 };
	SDL_Rect vSyncPos = { windowW / 2 - 60, windowH / 2 + 150 , 40,40 };
	SDL_Rect fullscreenbtPos = { windowW / 2 - 60, windowH / 2 + 50  , 40,40 };
	SDL_Rect creditPos = { windowW / 2 - 60, windowH / 2 + 200  , 120,20 };

	gcButtom = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 6, "   Start   ", btPos, this);
	exitButtom = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 7, "   Exit   ", exitPos, this);
	optionsButtom = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 8, "   Options   ", optionsPos, this);
	backButtom = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 9, "   Back   ", exitPos, this);
	musicButtom = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::SLIDER, 2, "   Sound   ", musicbtPos, this);
	fullScreenButtom = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::CHECKBOX, 11, "   Fullscreen   ", fullscreenbtPos, this);
	vSyncButtom = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::CHECKBOX, 12, "   Vsync   ", vSyncPos, this);
	creditButtom = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 13, "   Credits   ", creditPos, this);


	gcButtom->state = GuiControlState::DISABLED;
	exitButtom->state = GuiControlState::DISABLED;
	optionsButtom->state = GuiControlState::DISABLED;
	creditButtom->state = GuiControlState::DISABLED;
	return true;
	
}



// Called each loop iteration
bool TitleScreen::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool TitleScreen::Update(float dt)
{
	tittleTimerSec = titleTimer.ReadSec();
	tittleTimerMSec = titleTimer.ReadMSec();

	if (credits == true) 
	{
		creditsText = "Coding: Marco Baldan, Pau Mora                 " ;
		creditsText2 = "Art: Marco Baldan                  ";
		app->render->DrawText(creditsText, app->scene->windowW / 2 -550, app->scene->windowH / 2 , 600, 60);
		app->render->DrawText(creditsText2, app->scene->windowW / 2 - 550, app->scene->windowH / 2 + 80, 600, 60);
	}

	if (back == true) 
	{
		app->physics->DestroyPlatforms();
		app->entityManager->titlescreenreset = true;
		app->entityManager->DestroyAll();
		app->entityManager->titlescreenreset = false;

		app->map->CleanUp();	
		app->map->mapData.layers.Clear();
		app->scene->player->pendingToDestroy = true;

		app->physics->active = false;
		app->scene->active = false;		
		app->map->active = false;
		
		skip = true;
		stopesquizo = 0;
		startt = false;
		options = false;

		
		
		back = false;
	}

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN && app->scene->active == true)
	{
		if (options == false)
			options = true;
		else if (options == true)
			options = false;
	}
	if (options == true && app->scene->active == true) 
	{
		musicButtom->state = GuiControlState::NORMAL;
		fullScreenButtom->state = GuiControlState::NORMAL;
		vSyncButtom->state = GuiControlState::NORMAL;
	}

	if (delayTimer.ReadMSec() > 2500) {


		// This loop iterates through all the frames of the opening updating their positions and states
		ListItem<Frame*>* it = myFrames.start;
		while (it != NULL) {
			if (skip == false)
			it->data->Update(dt);


			it = it->next;
		}
	
		spearFrame->Update(dt);

		if (app->scene->active == false && startt == true) {
			app->scene->player = nullptr;
			app->physics->active = true;
			app->physics->Start();
			pugi::xml_node n = mynode.parent().child("scene");
			app->scene->active = true;
			app->scene->Awake(n);
			app->scene->Start();

			app->map->active = true;
			app->map->Start();


			app->guiManager->active = true;
			app->guiManager->Start();
			gcButtom->state = GuiControlState::DISABLED;
			exitButtom->state = GuiControlState::DISABLED;
			creditButtom->state = GuiControlState::DISABLED;
			optionsButtom->state = GuiControlState::DISABLED;
		}

		if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
		{
			skip = true;
		}

		if (skip == true) 
		{
			tittleTimerSec = 50;
			tittleTimerMSec = 50000;
		}

		if (tittleTimerSec > 44)
		{
			gcButtom->state = GuiControlState::NORMAL;
			exitButtom->state = GuiControlState::NORMAL;
			optionsButtom->state = GuiControlState::NORMAL;
			creditButtom->state = GuiControlState::NORMAL;
		}

		if (tittleTimerSec > 44 && tittleTimerSec < 49 || skip == true) {
			int alpha = 255;

			blinkCounter++;
			stopesquizo++;
			if (blinkCounter > 4) {
				alpha = 200;
				if (blinkCounter > 8) {
					blinkCounter = 0;
				}
			}
			if (stopesquizo > 300)
				alpha = 0;

			SDL_Rect ruct = SDL_Rect{ 0,0,640,360 };
			app->render->DrawTexture(finalFrame, -40, 0, false, &ruct, alpha);
		}
		
		if (tittleTimerSec > 47) {
			
			SDL_Rect ruct = SDL_Rect{ 0,0,640,185 };
			app->render->DrawTexture(Logo.texture, 0, 0, false, &ruct);
			if (tittleTimerMSec > 49000) {
				app->render->DrawTexture(Flame->texture, 0, 230, true, &Flame->currentAnim->GetCurrentFrame());
				Flame->currentAnim->Update();
			}
		}
	}
	if (optionsTittle == true)
	{
		gcButtom->state = GuiControlState::DISABLED;
		exitButtom->state = GuiControlState::DISABLED;
		optionsButtom->state = GuiControlState::DISABLED;
		creditButtom->state = GuiControlState::DISABLED;
		backButtom->state = GuiControlState::NORMAL;

		musicButtom->state = GuiControlState::NORMAL;
		fullScreenButtom->state = GuiControlState::NORMAL;
		vSyncButtom->state = GuiControlState::NORMAL;
		Mix_VolumeMusic(volume3);
		Mix_Volume(-1, volume3);

	}
	else 
	{
		backButtom->state = GuiControlState::DISABLED;
		//musicButtom->state = GuiControlState::DISABLED;
		//fullScreenButtom->state = GuiControlState::DISABLED;
		//vSyncButtom->state = GuiControlState::DISABLED;
	}
	if (optionsTittle == false && options == false) 
	{
		musicButtom->state = GuiControlState::DISABLED;
		fullScreenButtom->state = GuiControlState::DISABLED;
		vSyncButtom->state = GuiControlState::DISABLED;
	}


	if (app->scene->active == true) 
	{
		gcButtom->state = GuiControlState::DISABLED;
		exitButtom->state = GuiControlState::DISABLED;
		optionsButtom->state = GuiControlState::DISABLED;
		creditButtom->state = GuiControlState::DISABLED;
	}

	return true;
}

// Called each loop iteration
bool TitleScreen::PostUpdate()
{
	bool ret = true;

	if (exit == true)
		ret = false;

	return ret;
}

// Called before quitting
bool TitleScreen::CleanUp()
{
	LOG("Freeing scene");

	return true;
}

bool TitleScreen::OnGuiMouseClickEvent(GuiControl* control)
{
	fullscreenOnce++;
	vsyncOnce++;

	if (control->id == 6) {
		startt = true;
		
	}

	if (control->id == 8) {
		optionsTittle = true;

	}

	if (control->id == 9) {
		optionsTittle = false;

	}

	if (control->id == 7) {
		
		exit = true;
	}

	if (control->id == 11 && fullscreen == false && fullscreenOnce >= 1) {
		fullscreen = true;
		fullscreenOnce = 0;
		SDL_SetWindowFullscreen(app->win->window, SDL_WINDOW_FULLSCREEN);
	}


	if (control->id == 11 && fullscreen == true && fullscreenOnce >= 1) {


		fullscreen = false;
		fullscreenOnce = 0;
		uint heigth;
		uint width;

		app->win->GetWindowSize(width, heigth);

		SDL_SetWindowFullscreen(app->win->window, 0);
		SDL_SetWindowSize(app->win->window, width, heigth);
	}
	if (control->id == 13) {
		if (credits == true) {
			credits = false;

		}
		else if (credits == false) {
			credits = true;
		}

	}

	if (control->id == 12 && vSync == false && vsyncOnce >= 1) {
		vSync = true;
		vsyncOnce = 0;

	}
	else if (control->id == 12 && vSync == true && vsyncOnce >= 1) {
		vSync = false;
		vsyncOnce = 0;
	}

	return true;
}