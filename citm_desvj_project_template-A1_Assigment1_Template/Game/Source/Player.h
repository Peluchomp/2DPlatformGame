#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Entity.h"
#include "../Thunder.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "../Animation.h"
#include "../Spear.h"
#include "Timer.h"

enum Direction {
	RIGHT,
	LEFT
};
enum PowerLvl {
	NORMAL,
	MID,
	OP
};

struct SDL_Texture;

class PowerPopUp {
public:

	Animation* currentAnim;
	Animation defaultAnim;

	bool active = false;
	Timer myTimer;

	iPoint position;

};

class Player : public Entity
{
public:

	Player();
	
	virtual ~Player();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

	void Spawn(int Level);

	void LoadAnimations();

	void AttackHitBoxManagement();

	void AttackingLogic();

	bool SaveState(pugi::xml_node node);
	bool LoadState(pugi::xml_node node);

public:
	Direction myDir;

	float speed = 0.2f;
	const char* texturePath;
	SDL_Texture* texture = NULL;
	PhysBody* pbody;
	PhysBody* plegs;
	PhysBody* attackTrigger;

	Animation* currentAnim;

	Animation idle;
	Animation longIdle1;
	Animation longIdle2;
	Animation longRun;
	Animation playerRun;
	Animation epicSpawn;
	Animation quickSpawn;
	Animation groundAttack;
	Animation airAttack;
	Animation spearThrown;

	Animation* currentSpawnAnim;
	Animation poweUpAnim;
	Animation poweUpAnim2;
	Animation poweUpAnim3;

	//----Mid power Animations----//
	Animation mid_groundAttack;
	Animation mid_airAttack;
	Animation mid_spearThrow;

	//----OP power Animations-----//
	Animation op_airAttack;

	//----Spawn-Anim-----//
	bool spawning = true;

	Animation spawnFire;
	SDL_Rect SpawnRect = { 0,417, 80,130 };

	SDL_Rect orbMeter = { 20, 600, 0,20 };

	Animation Jump;
	Animation Fall;
	
	Spear *mySpear;
	Thunder* myThunder;
	PowerLvl power = PowerLvl::NORMAL;
	PowerPopUp powerMessage;

	b2Vec2 provisional = { 999,999 };

	int orbs = 0;

	bool dead = false;
	int pickCoinFxId;
	float gravity;
	float jumpDistance;
	bool isJumping;
	bool isGrounded;
	bool Attacking = false;
	bool groundBoost = false;
	bool idleState;
	float movementx;
	float angle_deg;
	float delta_y;
	float delta_x;
	int mousex;
	int mousey;
	bool godMode;
	bool fpsCap = true;

	bool thrown = false;
	bool SpearhasBeenThrown = false;
	Timer thrownCooldown;

	bool startIdle = false;
	Timer IdleTimer;

	bool powerTransition = false;

	//-------Audio effects--------//
	
	uint orbEffect;

};

#endif // __PLAYER_H__