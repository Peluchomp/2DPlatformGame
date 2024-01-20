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
#include "../Timer.h"


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
	// position where frame lands, time for interpolation, direction of interpolation, size of frame, time the frame is rendered, when does the frame start to get rendered
	Frame(iPoint pos, float t, Appearance appr, SDL_Rect size, float StayTime, float whenToStart) {
		this->t = t;
		desiredPos = pos;
		myAppearance = appr;
		startingPos = desiredPos;
		this->size = size;
		stayTime = StayTime + whenToStart;
		startValue = whenToStart;

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
	Timer nextTimer;

	Timer startTimer;
	float startValue;

	bool started = false;
	bool finished = false;

	int alpha = 0;

	SDL_Rect size;


	float t;
	float stayTime;

	Appearance myAppearance;

	void lerp() {
		
		startingPos.x + (t) * (desiredPos.x - startingPos.x);
		startingPos.y + (t) * (desiredPos.x - startingPos.x);
	}

	// is the frame has finished returns true , else return false
	void Update(float dt) {
		if (!finished) {
			if (!started) 
			{
				startTimer.Start();
				nextTimer.Start();
				started = true;
			}
			if (startTimer.ReadMSec() > startValue) {
				if (myAppearance != Appearance::FADE) {
					startingPos.x += (t * dt) * (desiredPos.x - startingPos.x);
					startingPos.y += (t * dt) * (desiredPos.y - startingPos.y);
					app->render->DrawTexture(texture, startingPos.x, startingPos.y, false, &size);

				}
				else {
					alpha += (t * dt) * (255 - alpha);
					app->render->DrawTexture(texture, startingPos.x, startingPos.y, false, &size, alpha);
				}
				if (nextTimer.ReadMSec() > stayTime) {
					finished = true;
				}
			}
		}
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

	SDL_Texture* finalFrame;
	Timer titleTimer;

	Icon Logo;
	Timer delayTimer;

	Icon* Flame;

	int blinkCounter = 0;

private:
	
	List<Frame*> myFrames;
	Frame* spearFrame;

};

#endif // __TITLESCREEN_H__