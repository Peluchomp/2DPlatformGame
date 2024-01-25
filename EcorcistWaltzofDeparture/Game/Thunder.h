#ifndef __THUNDER_H__
#define __THUNDER_H__

#include "Source/Entity.h"
#include "Source/Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"

struct SDL_Texture;

class Thunder : public Entity
{
public:

	Thunder();
	virtual ~Thunder();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	bool PreUpdate(float dt);

	void OnCollision(PhysBody* physA, PhysBody* physB);

	Animation* currentAnimation;
	Animation defaultAnim;
	Animation bossAnim;
	int pushBackSkip = 0;

	iPoint bossPos;

	SDL_Texture* texture = nullptr;

	PhysBody* _body;
	bool bossThunder = false;
	bool awake = false;

private:
	const char* texturePath;



};

#endif // __THUNDER_H__