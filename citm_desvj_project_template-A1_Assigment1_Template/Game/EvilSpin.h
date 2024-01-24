#ifndef __EVILSPIN_H__
#define __EVILSPIN_H__

#include "Source/Entity.h"
#include "Source/Point.h"
#include "SDL/include/SDL.h"
#include "Thunder.h"
#include "Animation.h"

struct SDL_Texture;

class EvilSpin : public Entity
{
public:

	EvilSpin();
	virtual ~EvilSpin();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	bool PreUpdate(float dt);

	void OnCollision(PhysBody* physA, PhysBody* physB);

	iPoint GenerateRandomDestination(const iPoint& initialPos, float distance);

	void SetDistance(int distance);

	Animation* currentAnimation;
	Animation defaultAnim;

	SDL_Texture* texture = nullptr;
	Timer taimer;
	PhysBody* pbody;
	PhysBody* eviltwinpbody;


	bool reachedPos;
	iPoint finalPos;
	iPoint inPos;

	Timer timeAlive;

	b2RevoluteJoint* revol;

	int distance = 70;

	float speed = 3;


private:
	const char* texturePath;



};

#endif // __EVILSPIN_H__