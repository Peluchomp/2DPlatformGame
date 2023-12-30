#ifndef __CHANDELIER_H__
#define __CHANDELIER_H__

#include "Source/Entity.h"
#include "Source/Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"
#include "../Timer.h"

struct SDL_Texture;

class Chandelier : public Entity
{
public:

	Chandelier();
	Chandelier(iPoint ogPosition);
	virtual ~Chandelier();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	bool PreUpdate(float dt);

	void PlayerStandingOnME();

	void OnCollision(PhysBody* physA, PhysBody* physB);

	Animation* currentAnimation;
	Animation defaultAnim;

	SDL_Texture* texture = nullptr;

	PhysBody* _body;
	PhysBody* plat_body;

	PhysBody* damage_body;

	bool awake = false;

	
private:
	const char* texturePath;

	Timer fallTimer;
	bool touched = false;
	bool fallen = false;
	bool toDestroy;

};

#endif // __CHANDELIER_H__