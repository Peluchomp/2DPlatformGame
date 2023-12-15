#ifndef __JORGE_H__
#define __JORGE_H__

#include "../Entity.h"
#include "../Point.h"
#include "SDL/include/SDL.h"
#include "../Map.h"

struct SDL_Texture;

class Jorge : public Entity
{
public:

	enum class State
	{
		IDLE,
		FLYING,
		DYING
	};


	Jorge();
	virtual ~Jorge();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

public:

	bool isPicked = false;

private:
	iPoint lastPlayerPosition;
	DynArray<iPoint> path;
	bool hasPath = false;
	int pathIndex = 0;


	fPoint fposition = fPoint(position.x, position.y);
	State state;
	fPoint initialPosition;


	int hp = 2;
	float timer;
	SDL_Texture* texture;
	SDL_Texture* texture2;
	const char* texturePath;
	PhysBody* pbody;
};

#endif // __ITEM_H__