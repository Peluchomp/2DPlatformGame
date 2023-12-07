#ifndef __ITEM_H__
#define __ITEM_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "Map.h"

struct SDL_Texture;

class Item : public Entity
{
public:

	enum class State
	{
		IDLE,
		FLYING,
		DYING
	};


	Item();
	virtual ~Item();

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



	float timer;
	SDL_Texture* texture;
	SDL_Texture* texture2;
	const char* texturePath;
	PhysBody* pbody;
};

#endif // __ITEM_H__