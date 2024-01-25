#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include "Source/App.h"
#include "Source/Audio.h"
#include "Source/Input.h"
#include "Source/Timer.h"
#include "SDL/include/SDL_rect.h"
#include "Source/Physics.h"
#define MAX_FRAMES 200

bool sameRect(SDL_Rect r1, SDL_Rect r2);

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
	int numOpportunities = 0;
	
	SDL_Rect* currentOpportunity = nullptr;


	bool opportunity = false;
	
	SDL_Scancode opportunityKey;

	// Audio stuff
	SDL_Rect* currentAudio = nullptr;

	int numEffects = 0;
	SDL_Rect soundFrames[MAX_FRAMES];
	int audioIndex = 0;
	uint soundEffects[MAX_FRAMES];
	bool hasPlayed[MAX_FRAMES] = {false};

private:
	Timer opportunityTimer;
	bool timerStarted = false;
	bool missedOpportunity = false;

	
	
	int opportunityIndex = 0;

	float currentFrame = 0.0f;
	int totalFrames = 0;
	
	int pingpongDirection = 1;
	
	bool mirror;

public:

	int GetOpportunityIndex() {
		return opportunityIndex;
	}

	void PushBack(const SDL_Rect& rect, bool opportunit = false, std::string effect = "", bool mirror = false )
	{
		frames[totalFrames++] = rect;
		this->mirror = mirror;

		if (opportunit == true) {
			
			opportunityFrames[opportunityIndex] = rect;
			currentOpportunity = &opportunityFrames[0];
			opportunityIndex++;
			
		}

		// Add effect to the animation's effect array
		if (effect != ""){
			soundEffects[audioIndex] = app->audio->LoadFx(effect.c_str());
			soundFrames[audioIndex] = rect;
			currentAudio = &soundFrames[0];
			audioIndex++;
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
		 currentAudio = &soundFrames[0];
		 for (int j = 0; j < MAX_FRAMES; ++j) { 
			 if (hasPlayed[j] == false) { break; }
			 hasPlayed[j] = false; 
		 }
		

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
		// opportunity logic

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

		// audios stufff

		SDL_Rect ra = { -1,-1,-1,-1 };
		if (currentAudio != nullptr) {
			ra = *currentAudio;

			int i = -1;
			SDL_Rect rb = GetCurrentFrame();
			if (sameRect(ra, rb)) {
				for (i = 0; i < MAX_FRAMES; ++i) /*Find index of current sound frame in array*/ {
					if (sameRect(soundFrames[i], ra)) {
						break;
					}
				}

				// Now the index is used to play the corresponding audio
				if (hasPlayed[i] == false) {
					app->audio->PlayFx(soundEffects[i]);
					hasPlayed[i] = true;
					currentAudio = &soundFrames[i + 1];

				}
				// it must be marked as already played to not play more than once

			}
		

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

	bool sameRect(SDL_Rect r1, SDL_Rect r2) {
		if (r1.x == r2.x && r1.y == r2.y && r1.w == r2.w && r1.h == r2.h) {
			return true;
		}
		else { return false; }
	}
};



//void CompareFrames(SDL_Rect frameA, SDL_Rect frameB, SDL_Rect frameB2 = NULL, SDL_Rect frameB3 = NULL) {
//
//	if(frameA.x)
//
//}

#endif