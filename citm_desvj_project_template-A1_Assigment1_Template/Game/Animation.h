#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include "App.h"
#include "Source/Input.h"
#include "Source/Timer.h"
#include "SDL/include/SDL_rect.h"
#define MAX_FRAMES 200

class Animation
{
public:
	float speed = 1.0f;
	SDL_Rect frames[MAX_FRAMES];
	bool loop = true;
	// Allows the animation to keep going back and forth
	bool pingpong = false;
	int loopCount = 0;
	float opportunityWindow = 0.3f;
	int opportunityFrame = 100;
	bool opportunity = false;
	
	SDL_Scancode opportunityKey;

private:
	Timer opportunityTimer;
	bool timerStarted = false;
	bool missedOpportunity = false;

	float currentFrame = 0.0f;
	int totalFrames = 0;
	
	int pingpongDirection = 1;
	
	bool mirror;

public:

	void PushBack(const SDL_Rect& rect, bool mirror = false)
	{
		frames[totalFrames++] = rect;
		this->mirror = mirror;
	}



	void Reset()
	{
		loopCount = 0;
		currentFrame = 0;
		 timerStarted = false;
		 missedOpportunity = false;
		 opportunity = false;
	}

	bool HasFinished()
	{
		if (opportunityFrame == 100 ) {
			return !loop && !pingpong && loopCount > 0;
		}
		else if (missedOpportunity) {
			return true;
		}
		else{
			return !loop && !pingpong && loopCount > 0;
		}
	}

	void Update()
	{
		if (((int)currentFrame <= opportunityFrame) || opportunity == true) { 
			currentFrame += speed * app->dt; 
		}
		else if(timerStarted == false) {
			opportunityTimer.Start();
			timerStarted = true;

		}
		if (currentFrame >= totalFrames)
		{
			currentFrame = (loop || pingpong) ? 0.0f : totalFrames - 1;
			++loopCount;

			if (pingpong)
				pingpongDirection = -pingpongDirection;
		}

		if (app->input->GetKey(opportunityKey) == KEY_DOWN && (int)currentFrame >= opportunityFrame && opportunity==false) {

			opportunity = true; // The combo to proceed to the 2nd part of the animation has been succefully performed
		}

		if (timerStarted && opportunityTimer.ReadMSec() >= opportunityWindow * 1000 && opportunity == false) /*The player has missed the opportunity for a combo*/ {
			missedOpportunity = true;

		}

	}
	bool GetMirror() const
	{
		return mirror;
	}

	const SDL_Rect& GetCurrentFrame() const
	{
		int actualFrame = currentFrame;
		if (pingpongDirection == -1)
			actualFrame = totalFrames - currentFrame;

		return frames[actualFrame];
	}
};

#endif