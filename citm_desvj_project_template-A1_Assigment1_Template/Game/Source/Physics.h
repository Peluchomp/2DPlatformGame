#pragma once
#include "Module.h"
#include "Entity.h"
#include "List.h"
#include "Box2D/Box2D/Box2D.h"

#define GRAVITY_X 0.0f
#define GRAVITY_Y -10.0f

#define PIXELS_PER_METER 50.0f // if touched change METER_PER_PIXEL too
#define METER_PER_PIXEL 0.02f // this is 1 / PIXELS_PER_METER !

#define METERS_TO_PIXELS(m) ((int) floor(PIXELS_PER_METER * m))
#define PIXEL_TO_METERS(p)  ((float) METER_PER_PIXEL * p)

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f

// Define category bits for different types of objects
const uint16_t PLAYER_CATEGORY_BIT = 0x0001;
const uint16_t ENEMY_CATEGORY_BIT = 0x0002;
const uint16_t GROUND_CATEGORY_BIT = 0x0004;

// Define mask bits for which they CAN collide with
const uint16_t PLAYER_MASK_BITS = ENEMY_CATEGORY_BIT | GROUND_CATEGORY_BIT;
const uint16_t ENEMY_MASK_BITS = PLAYER_CATEGORY_BIT | GROUND_CATEGORY_BIT;
const uint16_t GROUND_MASK_BITS = PLAYER_CATEGORY_BIT | ENEMY_CATEGORY_BIT | GROUND_CATEGORY_BIT;



// types of bodies
enum bodyType {
	DYNAMIC,
	STATIC,
	KINEMATIC
};

enum class ColliderType {
	PLAYER,
	ENEMY,
	PLATFORM,
	SPEAR,
	UNKNOWN,
	INSTAKILL,
	ORB,
	PLAYER_ATTACK,
	ENEMY_ATTACK
	// ..
};

// Small class to return to other modules to track position and rotation of physics bodies
class PhysBody
{
public:
	PhysBody() : listener(NULL), body(NULL), ctype(ColliderType::UNKNOWN)
	{}

	~PhysBody() {}

	void GetPosition(int& x, int& y) const;
	float GetRotation() const;
	bool Contains(int x, int y) const;
	int RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const;

	void SetPos(int x, int y) {
		b2Vec2 pos(x, y);
		body->SetTransform(pos, 0);
	}

public:
	bool active = true;

	bool pendingtoDestroy = false;

	int width, height;
	b2Body* body;
	Entity* listener;
	ColliderType ctype;

	Entity* myEntity;

};

// Module --------------------------------------
class Physics : public Module, public b2ContactListener // TODO
{
public:

	List<PhysBody*> physBodies;

	// Constructors & Destructors
	Physics();
	~Physics();

	// Main module steps
	bool Start();
	bool PreUpdate();
	bool PostUpdate();
	bool CleanUp();

	// Create basic physics objects
	PhysBody* CreateRectangle(int x, int y, int width, int height, bodyType type, ColliderType fixture, float density = 1.0f);
	PhysBody* CreateCircle(int x, int y, int radious, bodyType type, ColliderType fixture, bool sensor = false );
	PhysBody* CreateRectangleSensor(int x, int y, int width, int height, bodyType type, ColliderType fixture);
	PhysBody* CreateChain(int x, int y, int* points, int size, bodyType type, ColliderType fixture);


	void DestroyPlatforms();

	void DestroyObject(PhysBody* pbody);

	// b2ContactListener ---
	void BeginContact(b2Contact* contact);

	// Debug mode
	bool debug;


private:



	// Box2D World
	b2World* world;
public:

	b2Filter enemyFilterData;
	b2Filter playerFilterData;
	b2Filter groundFilterData;
};