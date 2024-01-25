#ifndef __HEALINGORB_H__
#define __HEALINGORB_H__

#include "Source/Entity.h"
#include "Source/Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"

struct SDL_Texture;

class HealingOrb : public Entity
{
public:

	HealingOrb();
	virtual ~HealingOrb();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	bool PreUpdate(float dt);

	void OnCollision(PhysBody* physA, PhysBody* physB);

	Animation* currentAnimation;
	Animation defaultAnim;

	SDL_Texture* texture = nullptr;

	PhysBody* _body;

	bool awake = false;

private:
	const char* texturePath;



};

#endif // __HEALINGORB_H__