#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "Point.h"
#include "SString.h"
#include "Input.h"
#include "Render.h"
#include "List.h"

class PhysBody;

enum Direction {
	RIGHT,
	LEFT
};

enum class EntityType
{
	PLAYER,
	MORGAN,
	MEGA_MORGAN,
	SPEAR,
	ORB,
	THUNDER,
	JORGE,
	CHANDELIER,
	ANGEL,
	CHECKPOINT,
	UNKNOWN,
	AELFRIC,
	EVILSPEARLIGHTNING
};



class Entity
{
public:

	Entity(EntityType type) : type(type), active(true) {}

	Entity() {};

	virtual bool Awake()
	{
		return true;
	}

	virtual bool Start()
	{
		return true;
	}

	virtual bool Update(float dt)
	{
		return true;
	}

	virtual bool CleanUp()
	{
		return true;
	}

	virtual bool LoadState(pugi::xml_node&)
	{
		return true;
	}

	virtual bool SaveState(pugi::xml_node&)
	{
		return true;
	}

	void Entity::Enable()
	{
		if (!active)
		{
			active = true;
			Start();
		}
	}

	void Entity::Disable()
	{
		if (active)
		{
			active = false;
			CleanUp();
		}
	}

	virtual void PlayerStandingOnME() {

	}

	virtual void OnCollision(PhysBody* physA, PhysBody* physB) {

	};
	virtual void OnCollisionEnd(PhysBody* physA, PhysBody* physB) {

	};
public:

	Direction myDir = Direction::LEFT;
	SString name;
	EntityType type;
	bool active = true;
	pugi::xml_node parameters;

	// Possible properties, it depends on how generic we
	// want our Entity class, maybe it's not renderable...
	iPoint position;
	bool renderable = true;
	int hp =1;
	bool pendingToDestroy = false;

	SDL_Texture* pathTexture = nullptr;

	int num = -1;

	iPoint ogPos;


	List<PhysBody*> myBodies;
};

#endif // __ENTITY_H__