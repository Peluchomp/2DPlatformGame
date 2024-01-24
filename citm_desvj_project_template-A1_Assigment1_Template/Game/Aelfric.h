#ifndef __AELFRIC_H__
#define __AELFRIC_H__

#include "Source/Entity.h"
#include "Source/Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"

struct SDL_Texture;


enum BossAttacks {

	SPIN,
	THUNDERS,
	GROUND_SPEARS

};

class evilSpear {

public:
	SDL_Texture* texture;

	PhysBody* pbody;
	PhysBody* rotationB;

	b2RevoluteJoint* revol;
	b2RevoluteJoint* revolMe;

	b2DistanceJoint* dist;

	int fakeRotation = 0;
	bool spinAttack = false;
	Timer spinTimer;

};

class Aelfric : public Entity
{
public:

	Aelfric();
	virtual ~Aelfric();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool PostUpdate();

	bool CleanUp();

	bool PreUpdate(float dt);

	void OnCollision(PhysBody* physA, PhysBody* physB);

	void DestroyFloatingSpears();

	void CreateSpears();

	void Teleport();

	bool startFight = false;

	Animation* currentAnimation;
	Animation walkingAnim;
	Animation magicAnim;

	Timer ChangePosTimer;

	SDL_Texture* texture = nullptr;

	b2RevoluteJoint* revol1;
	b2RevoluteJoint* revol2;
	Timer defendTimer;
	bool defending = false;

	bool destroySpears = false;

	b2Vec2 ogP1;
	b2Vec2 ogP2;
	b2Vec2 ogR1;
	b2Vec2 ogR2;
	SDL_Rect healthBar;

	PhysBody* _body;
	PhysBody* _body2;
	PhysBody* _body3;
	PhysBody* _detectionBody;

	evilSpear MrSpear;
	evilSpear MsSpear;

	BossAttacks currentAttack;
	bool startedSpin = false;
	bool spinTimeDecided = false;
	bool startedThunder = false;
	Timer attackChangeTimer;

	Timer floorSpearTimer;
	float floorSpearWait = 2;
	float spinSpearWait = 1;
	float spinSpearAmount = 2;
	float TeleportWait;
	
	bool hurt = false;
	Timer hurtTimer;

	bool first = true;
	bool awake = false;

private:
	const char* texturePath;



};

#endif // __Aelfric_H__