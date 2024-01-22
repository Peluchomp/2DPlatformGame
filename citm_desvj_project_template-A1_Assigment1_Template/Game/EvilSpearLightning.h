#ifndef __EVILSPEARLIGHTNING_H__
#define __EVILSPEARLIGHTNING_H__

#include "Source/Entity.h"
#include "Source/Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"

struct SDL_Texture;

class EvilSpearLightning : public Entity
{
public:

	EvilSpearLightning();
	virtual ~EvilSpearLightning();

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
	PhysBody* pbody;
	PhysBody* eviltwinpbody;

	bool awake = false;
	float startAtackTimer;
private:
	const char* texturePath;



};

#endif // __EVILSPEARLIGHTNING_H__