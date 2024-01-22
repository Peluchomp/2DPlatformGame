#ifndef __AELFRIC_H__
#define __AELFRIC_H__

#include "Source/Entity.h"
#include "Source/Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"

struct SDL_Texture;

class evilSpear {

public:
	SDL_Texture* texture;

	PhysBody* pbody;

	b2RevoluteJoint* revol;

	b2DistanceJoint* dist;

};

class Aelfric : public Entity
{
public:

	Aelfric();
	virtual ~Aelfric();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	bool PreUpdate(float dt);

	void OnCollision(PhysBody* physA, PhysBody* physB);

	Animation* currentAnimation;
	Animation walkingAnim;

	Timer ChangePosTimer;

	SDL_Texture* texture = nullptr;

	PhysBody* _body;

	evilSpear MrSpear;
	evilSpear MsSpear;

	bool awake = false;

private:
	const char* texturePath;



};

#endif // __Aelfric_H__