#ifndef __SPEAR_H__
#define __SPEAR_H__

#include "Source/Entity.h"
#include "Source/Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"

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

	bool isPicked = true;
	bool isSticked = false;
	int x;
	int y;
	bool started = true;
	PhysBody* pbody;
	PhysBody* ThePlatform;
	PhysBody* ThePlatformLong;
	bool platform = false;
	bool daPlatform = false;
	bool isThrown = false;
	bool spearUpgrade = false;
	float spearekis;
	float spearyi;
	SDL_Texture* texture;

	Animation* currentAnim;

	Animation form1Anim;
	Animation form2Anim;
	Animation form3Anim;

private:
	const char* texturePath;


	
	float angle;
	
};

#endif // __SPEAR_H__