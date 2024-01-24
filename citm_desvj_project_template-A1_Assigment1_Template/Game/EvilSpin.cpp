#include "EvilSpin.h"
#include "Source/App.h"
#include "Source/Textures.h"
#include "Source/Audio.h"
#include "Source/Input.h"
#include "Source/Render.h"
#include "Source/Scene.h"
#include "Source/Log.h"
#include "Source/Point.h"
#include "Source/Physics.h"
#include <cstdlib>
#include <ctime>

int getRandomNumber3(int min, int max) {
	// Seed the random number generator with the current time
	std::srand(static_cast<unsigned int>(std::time(0)));

	// Generate a random number between min and max (inclusive)
	return min + std::rand() % (max - min + 1);
}

EvilSpin::EvilSpin() : Entity(EntityType::EVILSPIN)
{
	name.Create("evilspearlightning");
}

EvilSpin::~EvilSpin() {}

void::EvilSpin::SetDistance(int distance) {
	this->distance = distance;
}

bool EvilSpin::Awake() {

	// CHANGE
	texture = app->tex->Load("Assets/Textures/FatherAelfric.png");

	pbody = app->physics->CreateRectangle(position.x, position.y, 16, 60, bodyType::DYNAMIC, ColliderType::PHYS2);
	pbody->ctype = ColliderType::ENEMY;
	pbody->body->SetGravityScale(0);
	inPos = position;

	finalPos = GenerateRandomDestination(inPos, distance);
	

	timeAlive.Start();

	myBodies.Add(pbody);

	return true;
}

bool EvilSpin::Start() {

	//initilize textures



	return true;
}
bool EvilSpin::PreUpdate(float dt)
{

	return  true;
}

bool EvilSpin::Update(float dt)
{
	
		if (!reachedPos)
		{
			b2Vec2 currentPos = pbody->body->GetPosition();
			b2Vec2 targetPos = b2Vec2(PIXEL_TO_METERS(finalPos.x), PIXEL_TO_METERS(finalPos.y));

			b2Vec2 direction = targetPos - currentPos;
			direction.Normalize();
			b2Vec2 velocity = speed * direction;

			pbody->body->SetLinearVelocity(velocity);


			// Hasit reached the destination ?
			if (b2Distance(currentPos, targetPos) < 1.0f && !reachedPos)
			{
				reachedPos = true;

				// Create a revolute joint to rotate around itself
				float anchorX = 0.0f;
				float anchorY = 0.0f;
				revol = app->physics->CreateRevoluteJoint(pbody, pbody, anchorX, anchorY, 0.0f, 5);
				revol->SetMotorSpeed(0.01f);
				pbody->body->SetAngularDamping(5);
			}
		}


		position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x);
		position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y);


		SDL_Rect spearRect = { 560,1,17,85 };
		app->render->DrawTexture(texture, position.x, position.y, false, &spearRect, 255, 1, 255, 255, 255, pbody->GetRotation());
	
	if ( timeAlive.ReadSec() > 4  && !pendingToDestroy) {
		pendingToDestroy = true;
		timeAlive.Start();
	}

	return true;
}

// distance in pixels
iPoint EvilSpin::GenerateRandomDestination(const iPoint& initialPos, float distance)
{

	float angle = static_cast<float>(getRandomNumber3(0, 360));
	angle *= 20;
	float radianAngle = DEGTORAD * angle;
	float destX = initialPos.x + distance * cosf(radianAngle);
	float destY = initialPos.y + distance * sinf(radianAngle);

	return iPoint(static_cast<int>(destX), static_cast<int>(destY));
}

bool EvilSpin::CleanUp()
{
	return true;
}

void EvilSpin::OnCollision(PhysBody* physA, PhysBody* physB) {



}