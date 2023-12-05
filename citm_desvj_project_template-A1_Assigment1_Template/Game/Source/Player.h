#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "../Animation.h"
#include "../Spear.h"
#include "Timer.h"

enum Direction {
	RIGHT,
	LEFT
};

struct SDL_Texture;

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

	bool SaveState(pugi::xml_node node);
	bool LoadState(pugi::xml_node node);

public:
	Direction myDir;

	float speed = 0.2f;
	const char* texturePath;
	SDL_Texture* texture = NULL;
	PhysBody* pbody;
	PhysBody* plegs;
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

	//----Spawn-Anim-----//
	bool spawning = true;

	Animation spawnFire;
	SDL_Rect SpawnRect = { 0,417, 80,130 };

	SDL_Rect orbMeter = { 20, 600, 0,20 };

	Animation Jump;
	Animation Fall;
	
	Spear *mySpear;

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

};

#endif // __PLAYER_H__