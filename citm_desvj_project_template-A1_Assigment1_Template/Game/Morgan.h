#ifndef __MORGAN_H__
#define __MORGAN_H__

#include "Source/Entity.h"
#include "Source/Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"
#include "Source/Map.h"

struct SDL_Texture;

class Morgan : public Entity
{
public:

	enum class State
	{
		IDLE,
		FLYING,
		DYING
	};


	Morgan();
	virtual ~Morgan();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void LoadAnimations();

	void HitBoxManagement();

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

	Animation* currentAnimation;
	Animation walking;
	Animation attacking;

	int hp = 2;
	float timer;
	SDL_Texture* texture;
	SDL_Texture* texture2;
	const char* texturePath;
	PhysBody* pbody;
	PhysBody* snakeBody;
};

#endif // __MORGAN_H__