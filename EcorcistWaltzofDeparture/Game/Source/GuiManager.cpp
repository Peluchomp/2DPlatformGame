#include "GuiManager.h"
#include "App.h"
#include "Textures.h"
#include "Optick/include/optick.h"
#include "GuiControlButton.h"
#include "../GuiSlider.h"
#include "../GuiCheckBox.h"
#include "../TitleScreen.h"
#include "Audio.h"
#include "Scene.h"
GuiManager::GuiManager() :Module()
{
	name.Create("guiManager");
}

GuiManager::~GuiManager() {}

bool GuiManager::Start()
{
	return true;
}

// L15: DONE1: Implement CreateGuiControl function that instantiates a new GUI control and add it to the list of controls
GuiControl* GuiManager::CreateGuiControl(GuiControlType type, int id, const char* text, SDL_Rect bounds, Module* observer, SDL_Rect sliderBounds)
{
	GuiControl* guiControl = nullptr;

	//Call the constructor according to the GuiControlType
	switch (type)
	{
	case GuiControlType::BUTTON:
		guiControl = new GuiControlButton(id, bounds, text);
		break;
	case GuiControlType::CHECKBOX:
		guiControl = new GuiCheckBox(id, bounds, text);
		guiControl->SetObserver(app->scene);
		break;
	case GuiControlType::SLIDER:
		guiControl = new GuiSlider(id, bounds, text);
		//guiControl->SetObserver(app->scene);
		break;
	}

	//Set the observer
	guiControl->observer = observer;

	// Created GuiControls are add it to the list of controls
	guiControlsList.Add(guiControl);

	return guiControl;
}

bool GuiManager::Update(float dt)
{	
	this->dt = dt;
	return true;
}

bool GuiManager::CleanUp()
{
	ListItem<GuiControl*>* control = guiControlsList.start;

	while (control != nullptr)
	{
		RELEASE(control);
	}

	return true;

	return false;
}


bool GuiManager::PostUpdate() {

	OPTICK_EVENT("Gui::PostUpdate");


	ListItem<GuiControl*>* control = guiControlsList.start;

	while (control != nullptr)
	{
		if (control->data->id != 50 && control->data->id != 43 && control->data->id != 5 && control->data->id != 1  && control->data->id != 3 && control->data->id != 4 )
			control->data->Update(dt);

		if (app->scene->active == true)
			if (((control->data->id == 50 && app->scene->player->deathScreen == true) || control->data->id == 43 || (control->data->id == 5 && app->scene->player->deathScreen == false)|| control->data->id == 4 || control->data->id == 3 || control->data->id == 1) && app->titleS->options == true)
				control->data->Update(dt);


		control = control->next;
	}

	return true;

}
