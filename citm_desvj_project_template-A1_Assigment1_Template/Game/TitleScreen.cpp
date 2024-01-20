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
	mynode = config;
	return true;
}

// Called before the first frame
bool TitleScreen::Start()
{
	pugi::xml_node config = mynode;
	if (active) {

		if (config.child("page")) {
			for (pugi::xml_node fNode = config.child("page").child("frame"); fNode; fNode = fNode.next_sibling("frame")) {
				SDL_Rect rect = { fNode.attribute("x").as_int(), fNode.attribute("y").as_int() ,fNode.attribute("w").as_int() ,fNode.attribute("h").as_int() };

				Frame* f = new Frame(iPoint(-60, 0), 0.005f , Appearance::RIGHTWARDS, rect);
				f->texture = app->tex->Load(config.child("page").attribute("texturepath").as_string());
				myFrames.Add(f);
			}



		}

	}
	return true;
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

