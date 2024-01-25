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
const uint16_t ENEMY_CATEGORY_BIT = 0x0009;
const uint16_t GROUND_CATEGORY_BIT = 0x0004;
const uint16_t PHYSIC_CATEGORY_BIT = 0x0005;
const uint16_t Float_PLAT_CATEGORY_BIT = 0x0002;
const uint16_t PHYSIC2_CATEGORY_BIT = 0x0008;
const uint16_t PHYSIC3_CATEGORY_BIT = 0x0007;


// Define mask bits for which they CAN collide with
const uint16_t PLAYER_MASK_BITS = ENEMY_CATEGORY_BIT | GROUND_CATEGORY_BIT |Float_PLAT_CATEGORY_BIT ;
const uint16_t ENEMY_MASK_BITS = PLAYER_CATEGORY_BIT | GROUND_CATEGORY_BIT;
const uint16_t GROUND_MASK_BITS = PLAYER_CATEGORY_BIT | ENEMY_CATEGORY_BIT ;
const uint16_t PHYSIC_MASK_BITS = PLAYER_CATEGORY_BIT; // ghost bodies only used for physics //
const uint16_t Float_PLAT_MASK_BIT = PLAYER_CATEGORY_BIT | GROUND_CATEGORY_BIT;
const uint16_t PHYSIC2_MASK_BITS = PLAYER_CATEGORY_BIT; // ghost bodies only used for physics //

// types of bodies
enum bodyType {
	DYNAMIC,
	STATIC,
	KINEMATIC
};

enum class ColliderType {
	PLAYER,
	ENEMY,
	PHYSIC_OBJ,
	PLATFORM,
	SPEAR,
	UNKNOWN,
	INSTAKILL,
	ORB,
	PLAYER_ATTACK,
	ENEMY_ATTACK,
	CHECKPOINT,
	BLACK_TRIGGER,
	PHYS2,
	HEALINGORB
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

	SDL_Rect collider;

	bool Intersects(SDL_Rect* r) const
	{

		if (r != nullptr && &collider != nullptr) {
			if (collider.w != 0 && collider.h != 0 && r->w != 0 && r->h != 0) {
				// returns true if there has been an intersection
				return (collider.x < r->x + r->w &&
					collider.x + collider.w > r->x &&
					collider.y < r->y + r->h &&
					collider.h + collider.y > r->y);
			}
		}


		else {
			return false;
		}
	}
	

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
	List<PhysBody*> corpses;

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

	b2RevoluteJoint* CreateRevolutionJoint(PhysBody* staticBody, PhysBody* moveableBody, float distance =-3.0f, bool enableMotor = true, bool limit = true, int motorSpeed = 150);
	b2RevoluteJoint* CreateRotationalJoint(b2Body* body);
	b2DistanceJoint* Physics::CreateDistanceJoint(b2Body* bodyA, b2Body* bodyB, float distance = -2.0f);
	b2PrismaticJoint* Physics::CreateHorizontalPrismaticJoint(b2Body* bodyA, b2Body* bodyB, float distance, bool inverted = false);

	b2RevoluteJoint* Physics::CreateRevoluteJoint(PhysBody* bodyA, PhysBody* bodyB, float relativeAnchorX, float relativeAnchorY, int speed = 20, float maxTorque = 900.0f);

	void DestroyPlatforms();

	void DestroyObject(PhysBody* pbody);

	// b2ContactListener ---
	void BeginContact(b2Contact* contact);

	virtual void EndContact(b2Contact* contact);
	// Debug mode
	bool debug;


public:

	bool breakAll = false;

	// Box2D World
	b2World* world;
public:

	b2Filter enemyFilterData;
	b2Filter playerFilterData;
	b2Filter groundFilterData;
	b2Filter physicFilterData;
	b2Filter floatPlatformFilterData;
	b2Filter physic2Filter;
	b2Filter physic3Filter;
};