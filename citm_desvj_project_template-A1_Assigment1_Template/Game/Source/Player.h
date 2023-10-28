#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "../Animation.h"
#include "../Spear.h"

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

	


public:
	Direction myDir;

	float speed = 0.2f;
	const char* texturePath;
	SDL_Texture* texture = NULL;
	PhysBody* pbody;
	PhysBody* plegs;
	Animation* currentAnim;

	Animation playerRun;
	
	Spear *mySpear;

	int pickCoinFxId;
	float gravity;
	float jumpDistance;
	bool isJumping;
	bool isGrounded;
	float movementx;
	float angle_deg;
	float delta_y;
	float delta_x;
	int mousex;
	int mousey;
};

#endif // __PLAYER_H__