#ifndef __FLOORSPEARS_H__
#define __FLOORSPEARS_H__

#include "Source/Entity.h"
#include "Source/Point.h"
#include "SDL/include/SDL.h"
#include "Thunder.h"
#include "Animation.h"

struct SDL_Texture;

class FloorSpears : public Entity
{
public:

	FloorSpears();
	virtual ~FloorSpears();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	bool PreUpdate(float dt);

	void OnCollision(PhysBody* physA, PhysBody* physB);

	Animation* currentAnimation;
	Animation defaultAnim;

	SDL_Texture* texture = nullptr;
	Timer taimer;
	Timer deathTaimer;

	PhysBody* pbody;

	bool spawn;

	bool awake = false;
	float startAtackTimer;
private:
	const char* texturePath;



};

#endif // __FLOORSPEARS_H__