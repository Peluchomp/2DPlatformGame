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
	SDL_Rect opportunityFrames[MAX_FRAMES];
	
	SDL_Rect* currentOpportunity = nullptr;

	bool opportunity = false;
	
	SDL_Scancode opportunityKey;

private:
	Timer opportunityTimer;
	bool timerStarted = false;
	bool missedOpportunity = false;

	int numOpportunities = 0;

	int opportunityIndex = 0;

	float currentFrame = 0.0f;
	int totalFrames = 0;
	
	int pingpongDirection = 1;
	
	bool mirror;

public:

	void PushBack(const SDL_Rect& rect, bool opportunit = false, bool mirror = false)
	{
		frames[totalFrames++] = rect;
		this->mirror = mirror;

		if (opportunit == true) {
			
			opportunityFrames[opportunityIndex] = rect;
			currentOpportunity = &opportunityFrames[0];
			opportunityIndex++;
			
		}
	}



	void Reset()
	{
		loopCount = 0;
		currentFrame = 0;
		 timerStarted = false;
		 missedOpportunity = false;
		 opportunity = false;
		 numOpportunities = 0;
		 currentOpportunity = &opportunityFrames[0];
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

	bool sameRect(SDL_Rect r1, SDL_Rect r2) {
		if (r1.x == r2.x && r1.y == r2.y && r1.w == r2.w && r1.h == r2.h) {
			return true;
		}
		else { return false; }
	}

	void Update()
	{
		SDL_Rect r2 = { -1,-1,-1,-1 };
		if (currentOpportunity != nullptr) {
			r2 = *currentOpportunity; 
		}
		SDL_Rect r = GetCurrentFrame();
		
		if (sameRect(r,r2) == false || opportunity == true) {
			currentFrame += speed * app->dt; 
			opportunity = false;
			timerStarted = false;
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

		if (app->input->GetKey(opportunityKey) == KEY_DOWN && sameRect(r,r2) == true && opportunity==false) {

			opportunity = true; // The combo to proceed to the 2nd part of the animation has been succefully performed
			numOpportunities++;
			currentOpportunity = &opportunityFrames[numOpportunities];
			opportunityTimer.Start();
		}

		if (timerStarted && opportunityTimer.ReadMSec() >= opportunityWindow * 1000 && opportunity == false) /*The player has missed the opportunity for a combo*/ {
			missedOpportunity = true;
			numOpportunities = 0;
			currentOpportunity = &opportunityFrames[numOpportunities];

		}

	}
	bool GetMirror() const
	{
		return mirror;
	}

	SDL_Rect& GetCurrentFrame() 
	{
		int actualFrame = currentFrame;
		if (pingpongDirection == -1)
			actualFrame = totalFrames - currentFrame;

		return frames[actualFrame];
	}
};

#endif