#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include "App.h"
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

private:
	float currentFrame = 0.0f;
	int totalFrames = 0;
	int loopCount = 0;
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
	}

	bool HasFinished()
	{
		return !loop && !pingpong && loopCount > 0;
	}

	void Update()
	{
		currentFrame += speed*app->dt;
		if (currentFrame >= totalFrames)
		{
			currentFrame = (loop || pingpong) ? 0.0f : totalFrames - 1;
			++loopCount;

			if (pingpong)
				pingpongDirection = -pingpongDirection;
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