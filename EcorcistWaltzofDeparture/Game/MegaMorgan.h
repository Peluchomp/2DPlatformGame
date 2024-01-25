#ifndef __MEGAMORGAN_H__
#define __MEGAMORGAN_H__

#include "Source/Entity.h"
#include "Source/Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"
#include "Source/Map.h"

struct SDL_Texture;

#define MORGAN_HP 3

class MegaMorgan : public Entity
{
public:

	enum class State
	{
		IDLE,
		FLYING,
		DYING
	};


	MegaMorgan();
	virtual ~MegaMorgan();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void LoadAnimations();

	void HitBoxManagement();

public:

	bool isPicked = false;

	uint enemyDeathFx;

private:
	iPoint lastPlayerPosition;
	DynArray<iPoint> path;
	bool hasPath = false;
	int pathIndex = 0;


	fPoint fposition = fPoint(position.x, position.y);
	State state;
	fPoint initialPosition;

	Animation* currentAnimation;
	Animation walking;
	Animation attacking;


	float timer;
	SDL_Texture* texture;
	SDL_Texture* texture2;
	const char* texturePath;
	PhysBody* pbody;
	PhysBody* AttackBody;

	b2RevoluteJoint* revol;
	SDL_Rect _chain1 = { 361, 2,7,7 };

};

#endif // __MEGAMORGAN_H__