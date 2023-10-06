#ifndef __SPEAR_H__
#define __SPEAR_H__

#include "Source/Entity.h"
#include "Source/Point.h"
#include "SDL/include/SDL.h"

struct SDL_Texture;

class Spear : public Entity
{
public:

	Spear();
	virtual ~Spear();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

public:

	bool isPicked = false;

private:

	SDL_Texture* texture;
	const char* texturePath;
	PhysBody* pbody;
	bool started;
	float angle;
};

#endif // __SPEAR_H__