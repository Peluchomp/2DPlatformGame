#ifndef __CHANDELIER_H__
#define __CHANDELIER_H__

#include "Source/Entity.h"
#include "Source/Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"
#include "../Timer.h"

struct SDL_Texture;

enum ChandelierType {
	PENDULUM,
	STATIONARY
};

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
	Animation stationaryAnim;

	SDL_Texture* texture = nullptr;

	PhysBody* jointBody;
	b2RevoluteJoint* revol;

	ChandelierType myType;

	Timer revolTimer;

	PhysBody* _body;
	PhysBody* plat_body;
	PhysBody* plat_sensor;

	bool destroyedJoint = false;
	bool destroying = false;

	Direction startingDir;

	PhysBody* damage_body;

	SDL_Rect _chain1 = { 361, 2,7,7 };
	SDL_Rect _chain2 = { 361, 13,7,7 };

	bool awake = false;

	
private:
	const char* texturePath;

	Timer fallTimer;
	bool touched = false;
	bool fallen = false;
	bool toDestroy;

};

#endif // __CHANDELIER_H__