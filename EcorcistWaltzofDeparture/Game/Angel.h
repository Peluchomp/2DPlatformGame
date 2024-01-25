#ifndef __ANGEL_H__
#define __ANGEL_H__

#include "Source/Entity.h"
#include "Source/Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"

struct SDL_Texture;

class Angel : public Entity
{
public:

	Angel();
	virtual ~Angel();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	bool PreUpdate(float dt);

	void OnCollision(PhysBody* physA, PhysBody* physB);

	void LoadAnimations();

	Animation* currentAnimation;
	Animation defaultAnim;

	SDL_Texture* texture = nullptr;

	PhysBody* _body;

	PhysBody* attackSensor;
	PhysBody* platBody;
	PhysBody* deathSensor;



	bool awake = false;

private:
	const char* texturePath;

public:

	Animation* currentAnim;
	Animation stationaryAnim;
	Animation attackAnim;
	Animation riseAnim;

	Timer restTimer;
	Timer coolDownTimer;
	b2Vec2 ogPlatPos;
	b2Vec2 ogHitPos;

	b2Vec2 eyeR = b2Vec2(315, 122);
	b2Vec2 eyeL = b2Vec2(328,122);

	bool attacked = false;
	bool swordDrop = false;
	bool cooldwonStated = false;
};

#endif // __ANGEL_H__