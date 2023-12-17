#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Entity.h"
#include "../Thunder.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "../Animation.h"
#include "../Spear.h"
#include "Timer.h"

#define INVINCIBILITY_MS 1600

enum PowerLvl {
	NORMAL,
	MID,
	OP
};

struct SDL_Texture;

class Icon {
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

	void StartIFrames();
	void ManageInvencibility();

	

	bool SaveState(pugi::xml_node node);
	bool LoadState(pugi::xml_node node);

public:
	float speed = 0.2f;
	const char* texturePath;
	SDL_Texture* texture = NULL;
	PhysBody* pbody;
	PhysBody* hurtBox;
	PhysBody* plegs;
	PhysBody* attackTrigger;

	PhysBody* op_attackTrigger;

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

	Animation hurtAnim;

	//----Mid power Animations----//
	Animation mid_groundAttack;
	Animation mid_airAttack;
	Animation mid_spearThrow;

	//----OP power Animations-----//
	Animation op_airAttack;
	Animation op_groundAttack;

	//----Spawn-Anim-----//
	bool spawning = true;

	Animation spawnFire;
	SDL_Rect SpawnRect = { 0,417, 80,130 };

	SDL_Rect orbMeter = { 20, 600, 0,20 };
	SDL_Rect healthBar = { 20,40,80,20 };

	Animation Jump;
	Animation Fall;

	Spear* mySpear;
	Thunder* myThunder;
	PowerLvl power = PowerLvl::NORMAL;
	Icon powerMessage;
	Icon noSpearIcon;
	Icon hurtIcon;
	Icon hurtScreen;
	SDL_Texture* hurtEffectText = nullptr;
	SDL_Texture* hurtScreenText = nullptr;

	b2Vec2 provisional = { 999,999 };

	int orbs = 0;


	//----HP stuff----//
	int hp = 4;
	bool iframes = false;
	bool dead = false;
	Timer invicibilityTimer;
	bool hurt = false;
	Timer knockTimer;
	Direction knockDir = LEFT;
	int invencibilityCounter = 0;


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

	bool _noSpearIcon;
	Timer spear_icon_timer;

	//-------Audio effects--------//

	uint orbEffect;
	uint noSpearEffect;

};

#endif // __PLAYER_H__