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

	bool PreUpdate(float dt);

	void OnCollision(PhysBody* physA, PhysBody* physB);

	void SpawnPlatform();

public:

	bool isPicked = false;

	int x;
	int y;
	bool started = true;
	PhysBody* pbody;
	PhysBody* ThePlatform;
	bool platform = false;
private:

	SDL_Texture* texture;
	const char* texturePath;


	
	float angle;
	
};

#endif // __SPEAR_H__