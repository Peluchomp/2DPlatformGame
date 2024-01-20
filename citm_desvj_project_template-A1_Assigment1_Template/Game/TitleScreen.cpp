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

	app->audio->PlayMusic(config.child("music").attribute("path").as_string());

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

				float interpolation = fNode.attribute("interpolation").as_float();
				if (interpolation == 0) { interpolation = 0.005f; }

				Frame* f = new Frame(iPoint(fNode.attribute("desiredX").as_int(), fNode.attribute("desiredY").as_int()), interpolation , aporanc, rect, fNode.attribute("timeStay").as_float()*1000, fNode.attribute("timeStart").as_float() * 1000);
				f->texture = app->tex->Load(pNode.attribute("texturepath").as_string());
				myFrames.Add(f);
			}



		}

	}
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
	ListItem<Frame*>* it = myFrames.start;
	while (it != NULL) {

		it->data->Update(dt);


		it = it->next;
	}
	

	if(app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && app->scene->active == false){

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

	}

	return true;
}

// Called each loop iteration
bool TitleScreen::PostUpdate()
{
	bool ret = true;

	

	return ret;
}

// Called before quitting
bool TitleScreen::CleanUp()
{
	LOG("Freeing scene");

	return true;
}

