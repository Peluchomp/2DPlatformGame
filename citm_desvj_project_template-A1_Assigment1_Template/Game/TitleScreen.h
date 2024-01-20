#ifndef __TITLESCREEN_H__
#define __TITLESCREEN_H__

#include "../Module.h"
#include "../Player.h"
#include "Morgan.h"
#include "Spear.h"
#include "Orb.h"
#include "Jorge.h"
#include "MegaMorgan.h"
#include "../GuiControl.h"
#include "../GuiControlButton.h"

struct SDL_Texture;

enum Appearance {
	LEFTWARDS,
	RIGHTWARDS,
	DOWN,
	UP,
	FADE
};

class Frame {
public:

	Frame(iPoint start, float t, Appearance appr, SDL_Rect size) {
		this->t = t;
		desiredPos = start;
		myAppearance = appr;
		startingPos = desiredPos;
		this->size = size;

		switch (myAppearance) {
		case LEFTWARDS:
			startingPos.x = -300;
			break;
		case RIGHTWARDS:
			startingPos.x = 700;
			break;
		case UP:
			startingPos.y = -300;
			break;
		case DOWN:
			startingPos.y = 600;
			break;
		}

	}

	SDL_Texture* texture;
	iPoint desiredPos;
	iPoint startingPos;

	SDL_Rect size;
	float t;

	Appearance myAppearance;

	void lerp() {
		
		startingPos.x + (t) * (desiredPos.x - startingPos.x);
		startingPos.y + (t) * (desiredPos.x - startingPos.x);
	}

	void Update(float dt) {

		startingPos.x += (t * dt) * (desiredPos.x - startingPos.x);
		startingPos.y += (t * dt) * (desiredPos.y - startingPos.y);
		app->render->DrawTexture(texture, startingPos.x, startingPos.y, false, &size);

	}

};

class TitleScreen : public Module
{
public:

	TitleScreen();

	// Destructor
	virtual ~TitleScreen();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	pugi::xml_node mynode;


private:
	
	List<Frame*> myFrames;

};

#endif // __TITLESCREEN_H__