#include "Physics.h"
#include "Input.h"
#include "app.h"
#include "Log.h"
#include "math.h"
#include "SDL/include/SDL_keycode.h"
#include "Defs.h"
#include "Log.h"
#include "Render.h"
#include "Player.h"
#include "Window.h"
#include "Optick/include/optick.h"
#include "Box2D/Box2D/Box2D.h"
#include "List.h"
#include "Scene.h"

// Tell the compiler to reference the compiled Box2D libraries
#ifdef _DEBUG
#pragma comment( lib, "../Game/Source/External/Box2D/libx86/DebugLib/Box2D.lib" )
#else
#pragma comment( lib, "../Game/Source/External/Box2D/libx86/ReleaseLib/Box2D.lib" )
#endif

Physics::Physics() : Module()
{
	// Initialise all the internal class variables, at least to NULL pointer
	world = NULL;
	debug = true;
}

// Destructor
Physics::~Physics()
{
	// You should do some memory cleaning here, if required
}

bool Physics::Start()
{
	if (active) {
		LOG("Creating Physics 2D environment");

		// Create a new World
		world = new b2World(b2Vec2(GRAVITY_X, -GRAVITY_Y));

		// Set this module as a listener for contacts
		world->SetContactListener(this);

		// Set the filter data for the player's fixture

		playerFilterData.categoryBits = PLAYER_CATEGORY_BIT;
		playerFilterData.maskBits = PLAYER_MASK_BITS;
		playerFilterData.groupIndex = 0;

		// Set the filter data for the enemy's fixture

		enemyFilterData.categoryBits = ENEMY_CATEGORY_BIT;
		enemyFilterData.maskBits = ENEMY_MASK_BITS;
		enemyFilterData.groupIndex = 0;

		// Set the filter data for the ground's fixture
		groundFilterData.categoryBits = GROUND_CATEGORY_BIT;
		groundFilterData.maskBits = GROUND_MASK_BITS | PHYSIC_MASK_BITS | Float_PLAT_MASK_BIT;
		groundFilterData.groupIndex = 0;

		physicFilterData.categoryBits = PHYSIC_CATEGORY_BIT;
		physicFilterData.maskBits = PHYSIC_MASK_BITS;
		physicFilterData.groupIndex = 0;


		//floatPlatformFilterData.categoryBits = Float_PLAT_CATEGORY_BIT;
		//floatPlatformFilterData.maskBits = Float_PLAT_MASK_BIT |  GROUND_MASK_BITS;
		//floatPlatformFilterData.groupIndex = 0;

		debug = false;
	}
	return true;
}


bool Physics::PreUpdate()
{
	OPTICK_FRAME("Physics")
	bool ret = true;

	// Step (update) the World
	// WARNING: WE ARE STEPPING BY CONSTANT 1/60 SECONDS!
	if (app->scene->player->options == false)
	world->Step(1.0f / 60.0f, 6, 2);

	// Because Box2D does not automatically broadcast collisions/contacts with sensors, 
	// we have to manually search for collisions and "call" the equivalent to the ModulePhysics::BeginContact() ourselves...
	for (b2Contact* c = world->GetContactList(); c; c = c->GetNext())
	{
		// For each contact detected by Box2D, see if the first one colliding is a sensor
		if (c->IsTouching() && c->GetFixtureA()->IsSensor())
		{
			// If so, we call the OnCollision listener function (only of the sensor), passing as inputs our custom PhysBody classes
			PhysBody* pb1 = (PhysBody*)c->GetFixtureA()->GetBody()->GetUserData();
			PhysBody* pb2 = (PhysBody*)c->GetFixtureB()->GetBody()->GetUserData();

			if (pb1->pendingtoDestroy) {
				DestroyObject(pb1);
			}

			if (pb1->ctype == ColliderType::PLAYER && pb2->ctype == ColliderType::ENEMY_ATTACK || pb1->ctype == ColliderType::ENEMY_ATTACK && pb2->ctype == ColliderType::PLAYER) {
				LOG("Here");
			}
			
			if (pb1 && pb2 && pb1->listener && pb1->active && pb2->active)
				pb1->listener->OnCollision(pb1, pb2);
		}
	}

	return ret;
}

PhysBody* Physics::CreateRectangle(int x, int y, int width, int height, bodyType type, ColliderType fixture, float density)
{
	b2BodyDef body;

	if (type == DYNAMIC) body.type = b2_dynamicBody;
	if (type == STATIC) body.type = b2_staticBody;
	if (type == KINEMATIC) body.type = b2_kinematicBody;

	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	//body.fixedRotation = true;

	b2Body* b = world->CreateBody(&body);
	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef Fixture;

	
	Fixture.shape = &box;
	Fixture.density = density;
	b->ResetMassData();


	switch (fixture) {

	case(ColliderType::ENEMY):
		Fixture.filter = enemyFilterData;
		break;
	case(ColliderType::PLAYER):
		Fixture.filter = playerFilterData;
		break;
	case(ColliderType::PLATFORM):
		Fixture.filter = groundFilterData;
		break;
	case(ColliderType::ENEMY_ATTACK):
		Fixture.filter = enemyFilterData;
		break;
	case(ColliderType::PHYSIC_OBJ):
		Fixture.filter = physicFilterData;
		break;
	case(ColliderType::UNKNOWN):
		Fixture.filter = floatPlatformFilterData;
		break;
	}

	b->CreateFixture(&Fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = width * 0.5f;
	pbody->height = height * 0.5f;

	physBodies.Add(pbody);
	
	return pbody;
}



PhysBody* Physics::CreateCircle(int x, int y, int radious, bodyType type, ColliderType fixture, bool sensor)
{
	// Create BODY at position x,y
	b2BodyDef body;

	if (type == DYNAMIC) body.type = b2_dynamicBody;
	if (type == STATIC) body.type = b2_staticBody;
	if (type == KINEMATIC) body.type = b2_kinematicBody;

	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	// Add BODY to the world
	b2Body* b = world->CreateBody(&body);

	// Create SHAPE
	b2CircleShape circle;
	circle.m_radius = PIXEL_TO_METERS(radious);

	// Create FIXTURE
	b2FixtureDef Fixture;
	Fixture.shape = &circle;
	Fixture.density = 1.0f;
	Fixture.isSensor = sensor;
	b->ResetMassData();


	switch (fixture) {

	case(ColliderType::ENEMY):
		Fixture.filter = enemyFilterData;
		break;
	case(ColliderType::PLAYER):
		Fixture.filter = playerFilterData;
		break;
	case(ColliderType::PLATFORM):
		Fixture.filter = groundFilterData;
		break;
	case(ColliderType::ENEMY_ATTACK):
		Fixture.filter = enemyFilterData;
		break;
	case(ColliderType::PHYSIC_OBJ):
		Fixture.filter = physicFilterData;
		break;
	}

	// Add fixture to the BODY
	b->CreateFixture(&Fixture);

	// Create our custom PhysBody class
	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = radious * 0.5f;
	pbody->height = radious * 0.5f;

	physBodies.Add(pbody);
	// Return our PhysBody class
	return pbody;
}



PhysBody* Physics::CreateRectangleSensor(int x, int y, int width, int height, bodyType type, ColliderType fixture)
{
	// Create BODY at position x,y
	b2BodyDef body;
	if (type == DYNAMIC) body.type = b2_dynamicBody;
	if (type == STATIC) body.type = b2_staticBody;
	if (type == KINEMATIC) body.type = b2_kinematicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	// Add BODY to the world
	b2Body* b = world->CreateBody(&body);

	// Create SHAPE
	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	// Create FIXTURE
	b2FixtureDef Fixture;
	Fixture.shape = &box;
	Fixture.density = 1.0f;
	Fixture.isSensor = true;


	switch (fixture) {

	case(ColliderType::ENEMY):
		Fixture.filter = enemyFilterData;
		break;
	case(ColliderType::PLAYER):
		Fixture.filter = playerFilterData;
		break;
	case(ColliderType::PLATFORM):
		Fixture.filter = groundFilterData;
		break;
	case(ColliderType::ENEMY_ATTACK):
		Fixture.filter = enemyFilterData;
		break;
	case(ColliderType::PHYSIC_OBJ):
		Fixture.filter = physicFilterData;
		break;
	}

	// Add fixture to the BODY
	b->CreateFixture(&Fixture);

	// Create our custom PhysBody class
	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = width;
	pbody->height = height;

	physBodies.Add(pbody);
	// Return our PhysBody class
	return pbody;
}

PhysBody* Physics::CreateChain(int x, int y, int* points, int size, bodyType type, ColliderType fixture)
{
	// Create BODY at position x,y
	b2BodyDef body;
	if (type == DYNAMIC) body.type = b2_dynamicBody;
	if (type == STATIC) body.type = b2_staticBody;
	if (type == KINEMATIC) body.type = b2_kinematicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	// Add BODY to the world
	b2Body* b = world->CreateBody(&body);

	// Create SHAPE
	b2ChainShape shape;
	b2Vec2* p = new b2Vec2[size / 2];
	for (uint i = 0; i < size / 2; ++i)
	{
		p[i].x = PIXEL_TO_METERS(points[i * 2 + 0]);
		p[i].y = PIXEL_TO_METERS(points[i * 2 + 1]);
	}
	shape.CreateLoop(p, size / 2);

	// Create FIXTURE
	b2FixtureDef Fixture;
	Fixture.shape = &shape;


	switch (fixture) {

	case(ColliderType::ENEMY):
		Fixture.filter = enemyFilterData;
		break;
	case(ColliderType::PLAYER):
		Fixture.filter = playerFilterData;
		Fixture.density = 0.001f;
		break;
	case(ColliderType::PLATFORM):
		Fixture.filter = groundFilterData;
		break;
	case(ColliderType::ENEMY_ATTACK):
		Fixture.filter = enemyFilterData;
		break;
	case(ColliderType::PHYSIC_OBJ):
		Fixture.filter = physicFilterData;
		break;
	}

	// Add fixture to the BODY
	b->CreateFixture(&Fixture);


	// Clean-up temp array
	delete[] p;

	// Create our custom PhysBody class
	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = pbody->height = 0;

	physBodies.Add(pbody);
	// Return our PhysBody class
	return pbody;
}

// 
bool Physics::PostUpdate()
{
	OPTICK_EVENT()
	bool ret = true;

	// Activate or deactivate debug mode
	if (app->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
		debug = !debug;
	
	//  Iterate all objects in the world and draw the bodies
	if (debug)
	{
		for (b2Body* b = world->GetBodyList(); b; b = b->GetNext())
		{
			
			for (ListItem<PhysBody*>* p =  physBodies.start;  p != nullptr ; p = p->next) {

				if (p->data->body == b && p->data->active) {
					for (b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
			
						switch (f->GetType())
						{
							// Draw circles ------------------------------------------------
						case b2Shape::e_circle:
						{
							b2CircleShape* shape = (b2CircleShape*)f->GetShape();
							uint width, height;
							app->win->GetWindowSize(width, height);
							b2Vec2 pos = f->GetBody()->GetPosition();
							app->render->DrawCircle(METERS_TO_PIXELS(pos.x), METERS_TO_PIXELS(pos.y), METERS_TO_PIXELS(shape->m_radius) * app->win->GetScale(), 255, 255, 255);
						}
						break;

						// Draw polygons ------------------------------------------------
						case b2Shape::e_polygon:
						{
							b2PolygonShape* polygonShape = (b2PolygonShape*)f->GetShape();
							int32 count = polygonShape->GetVertexCount();
							b2Vec2 prev, v;

							for (int32 i = 0; i < count; ++i)
							{
								v = b->GetWorldPoint(polygonShape->GetVertex(i));
								if (i > 0)
									app->render->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 255, 255, 100);
								//p->data->collider = { METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y) };
								prev = v;
							}

							v = b->GetWorldPoint(polygonShape->GetVertex(0));
							app->render->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 255, 100, 100);
						}
						break;

						// Draw chains contour -------------------------------------------
						case b2Shape::e_chain:
						{
							b2ChainShape* shape = (b2ChainShape*)f->GetShape();
							b2Vec2 prev, v;

							for (int32 i = 0; i < shape->m_count; ++i)
							{
								v = b->GetWorldPoint(shape->m_vertices[i]);
								if (i > 0)
									app->render->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 100, 255, 100);
								prev = v;
							}

							v = b->GetWorldPoint(shape->m_vertices[0]);
							app->render->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 100, 255, 100);
						}
						break;

						// Draw a single segment(edge) ----------------------------------
						case b2Shape::e_edge:
						{
							b2EdgeShape* shape = (b2EdgeShape*)f->GetShape();
							b2Vec2 v1, v2;

							v1 = b->GetWorldPoint(shape->m_vertex0);
							v1 = b->GetWorldPoint(shape->m_vertex1);
							app->render->DrawLine(METERS_TO_PIXELS(v1.x), METERS_TO_PIXELS(v1.y), METERS_TO_PIXELS(v2.x), METERS_TO_PIXELS(v2.y), 100, 100, 255);
						}
						break;
					}
				}
				

			}

			

			
		}
	}


	return ret;
}

b2RevoluteJoint* Physics::CreateRevolutionJoint(PhysBody* staticBody, PhysBody* moveableBody, float distance) {

	b2RevoluteJointDef revoluteJointDef;
	revoluteJointDef.bodyA = staticBody->body;
	revoluteJointDef.bodyB = moveableBody->body;
	revoluteJointDef.localAnchorA.Set(0.0f, 0.0f); // Anchor point on static body (in local coordinates)
	revoluteJointDef.localAnchorB.Set(0.0f, distance); // Anchor point on movable body (in local coordinates)
	revoluteJointDef.enableLimit = true; // Set to true if you want to limit the rotation range
	revoluteJointDef.enableMotor = true; // Set to true to enable the joint motor
	revoluteJointDef.motorSpeed = 12; // Set motor speed (adjust as needed)
	revoluteJointDef.maxMotorTorque = 100; // Set maximum motor torque (adjust as needed)
	revoluteJointDef.lowerAngle =  -(M_PI/5)/* Set lower angle limit if enableLimit is true */;
	revoluteJointDef.upperAngle = + M_PI/5/* Set upper angle limit if enableLimit is true */;

	// Create the revolute joint
	b2RevoluteJoint* revoluteJoint = (b2RevoluteJoint*)world->CreateJoint(&revoluteJointDef);

	return revoluteJoint;
	
}

b2DistanceJoint* Physics::CreateDistanceJoint(b2Body* bodyA, b2Body* bodyB, float distance) {
	b2DistanceJointDef distanceJointDef;
	distanceJointDef.bodyA = bodyA;
	distanceJointDef.bodyB = bodyB;
	distanceJointDef.localAnchorA.Set(0.0f, 0.0f);                // Anchor point on bodyA (in local coordinates)
	distanceJointDef.localAnchorB.Set(0.0f, 0.0f);

	// Anchor point on bodyB (in local coordinates)
	distanceJointDef.length = distance;                            // Desired distance between bodies
	distanceJointDef.frequencyHz = 2.0f;                          // Adjust frequency (optional)
	distanceJointDef.dampingRatio = 0.5f;                          // Adjust damping ratio (optional)
	distanceJointDef.collideConnected = true;                      // Adjust collision behavior if needed

	b2DistanceJoint* distanceJoint = (b2DistanceJoint*)world->CreateJoint(&distanceJointDef);

	return distanceJoint;
}

b2PrismaticJoint* Physics::CreateHorizontalDistanceJoint(b2Body* bodyA, b2Body* bodyB, float distance) {

	b2DistanceJointDef distanceJointDef;
	distanceJointDef.Initialize(bodyA, bodyB, bodyA->GetWorldCenter(), bodyB->GetWorldCenter());
	distanceJointDef.length = distance;
	b2DistanceJoint* distanceJoint = (b2DistanceJoint*)world->CreateJoint(&distanceJointDef);

	// Now create a prismatic joint to restrict movement to the horizontal axis
	b2PrismaticJointDef prismaticJointDef;
	prismaticJointDef.Initialize(bodyA, bodyB, bodyA->GetWorldCenter(), b2Vec2(1.0f, 0.0f));
	prismaticJointDef.localAxisA.Normalize();  // Ensure the axis is a unit vector
	b2PrismaticJoint* prismaticJoint = (b2PrismaticJoint*)world->CreateJoint(&prismaticJointDef);

	return prismaticJoint;
}

// Called before quitting
bool Physics::CleanUp()
{
	LOG("Destroying physics world");

	// Delete the whole physics world!
	delete world;

	return true;
}

// Callback function to collisions with Box2D
void Physics::BeginContact(b2Contact* contact)
{
	
	// Call the OnCollision listener function to bodies A and B, passing as inputs our custom PhysBody classes
	PhysBody* physA = (PhysBody*)contact->GetFixtureA()->GetBody()->GetUserData();
	PhysBody* physB = (PhysBody*)contact->GetFixtureB()->GetBody()->GetUserData();

	if (physA->ctype == ColliderType::SPEAR && physB->ctype == ColliderType::PLAYER || physA->ctype == ColliderType::PLAYER && physB->ctype == ColliderType::SPEAR)
		return;

	if (physA && physA->listener != NULL)
		physA->listener->OnCollision(physA, physB);

	if (physB && physB->listener != NULL)
		physB->listener->OnCollision(physB, physA);
}

void Physics::EndContact(b2Contact* contact) {

	PhysBody* physA = (PhysBody*)contact->GetFixtureA()->GetBody()->GetUserData();
	PhysBody* physB = (PhysBody*)contact->GetFixtureB()->GetBody()->GetUserData();

	if (physA && physA->listener != NULL)
		physA->listener->OnCollisionEnd(physA, physB);

	if (physB && physB->listener != NULL)
		physB->listener->OnCollisionEnd(physB, physA);

}
//--------------- PhysBody

void PhysBody::GetPosition(int& x, int& y) const
{
	b2Vec2 pos = body->GetPosition();
	x = METERS_TO_PIXELS(pos.x) - (width);
	y = METERS_TO_PIXELS(pos.y) - (height);
}

float PhysBody::GetRotation() const
{
	return RADTODEG * body->GetAngle();
}

bool PhysBody::Contains(int x, int y) const
{
	b2Vec2 p(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	const b2Fixture* fixture = body->GetFixtureList();

	while (fixture != NULL)
	{
		if (fixture->GetShape()->TestPoint(body->GetTransform(), p) == true)
			return true;
		fixture = fixture->GetNext();
	}

	return false;
}

int PhysBody::RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const
{
	int ret = -1;

	b2RayCastInput input;
	b2RayCastOutput output;

	input.p1.Set(PIXEL_TO_METERS(x1), PIXEL_TO_METERS(y1));
	input.p2.Set(PIXEL_TO_METERS(x2), PIXEL_TO_METERS(y2));
	input.maxFraction = 1.0f;

	const b2Fixture* fixture = body->GetFixtureList();

	while (fixture != NULL)
	{
		if (fixture->GetShape()->RayCast(&output, input, body->GetTransform(), 0) == true)
		{
			// do we want the normal ?

			float fx = x2 - x1;
			float fy = y2 - y1;
			float dist = sqrtf((fx * fx) + (fy * fy));

			normal_x = output.normal.x;
			normal_y = output.normal.y;

			return output.fraction * dist;
		}
		fixture = fixture->GetNext();
	}

	return ret;
}

void Physics::DestroyPlatforms() {

	ListItem<PhysBody*>* item;
	item = app->map->killers.start;

	while (item) {
				DestroyObject(item->data);
		
		item = item->next;
	}


}

void Physics::DestroyObject(PhysBody* pbody) {

	world->DestroyBody(pbody->body);
}