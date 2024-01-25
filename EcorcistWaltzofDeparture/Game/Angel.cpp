#include "Angel.h"
#include "Source/App.h"
#include "Source/Textures.h"
#include "Source/Audio.h"
#include "Source/Input.h"
#include "Source/Render.h"
#include "Source/Scene.h"
#include "Source/Log.h"
#include "Source/Point.h"
#include "Source/Physics.h"

Angel::Angel() : Entity(EntityType::ANGEL)
{
	name.Create("angel");
}

Angel::~Angel() {}

bool Angel::Awake() {

	// Angels are loaded from the Map


	return true;
}

bool Angel::Start() {

	//initilize textures
	texture = app->entityManager->angel_tex;


	LoadAnimations();
	stationaryAnim.PushBack({ 2,2,636,571 });
	currentAnim = &stationaryAnim;

	attackAnim.loop = false;
	stationaryAnim.loop = true;
	riseAnim.loop = false;

	attackSensor = app->physics->CreateRectangleSensor(position.x +326, position.y +400, 160, 306, bodyType::DYNAMIC, ColliderType::ENEMY);
	myBodies.Add(attackSensor);
	attackSensor->body->SetGravityScale(0);
	
	platBody = app->physics->CreateRectangle(position.x + 326, position.y + 220, 160, 40, bodyType::DYNAMIC, ColliderType::PLATFORM, 3000);
	platBody->ctype = ColliderType::PLATFORM;
	myBodies.Add(platBody);
	platBody->body->SetSleepingAllowed(false);
	platBody->body->SetGravityScale(0.0f);
	platBody->body->SetFixedRotation(true);

	ogPlatPos = platBody->body->GetPosition();

	deathSensor = app->physics->CreateRectangle(position.x + 326, position.y + 300, 90, 110, bodyType::DYNAMIC, ColliderType::PHYSIC_OBJ,3000);
	deathSensor->ctype = ColliderType::ENEMY_ATTACK;
	deathSensor->body->SetGravityScale(0);
	myBodies.Add(deathSensor);

	ogHitPos = deathSensor->body->GetPosition();

	coolDownTimer.Start();

	return true;
}
bool Angel::PreUpdate(float dt)
{

	return  true;
}

bool Angel::Update(float dt)
{
	int x, y;
	attackSensor->GetPosition(x, y);
	attackSensor->collider = SDL_Rect{ x + 160 / 2, y + 306 / 2, 160, 306 };
	
	int PlayerX = app->scene->player->position.x + (112 / 2);

	if (!attacked &&  attackSensor->Intersects(&app->scene->player->pbody->collider) && coolDownTimer.ReadMSec() > 1000) {
		// if he has detected the player beneath him and is in position to attack

		LOG("Found player");
		// Change bodies to dynamic and use gravity to fall
		currentAnim = &attackAnim;
		riseAnim.Reset();
		platBody->body->SetType(b2_dynamicBody);
		platBody->body->SetGravityScale(0.8f);
		cooldwonStated = false;
		deathSensor->body->SetType(b2_dynamicBody);
		deathSensor->body->SetGravityScale(0.8f);

		attacked = true;

	}
	if (attackAnim.HasFinished() && !swordDrop) {
		// after the sword has fallen sya in pos for a bit, change bodies to static to prevent from falling more
		platBody->body->SetGravityScale(0.0f);
		platBody->body->SetType(b2_staticBody);

		deathSensor->body->SetGravityScale(0.0f);
		deathSensor->body->SetType(b2_staticBody);
		swordDrop = true;
		restTimer.Start();
	}

	if (swordDrop && restTimer.ReadMSec() > 1500) {
		// after having lowered the sword set an upwards linear velocity to lift up the sword again
		currentAnim = &riseAnim;
		platBody->body->SetType(b2_dynamicBody);
		platBody->body->SetGravityScale(-0.55f);

		deathSensor->body->SetType(b2_dynamicBody);
		deathSensor->body->SetGravityScale(-0.55f);

	}

	if (riseAnim.HasFinished()) {
		// after the sword has risen make sure all transforms go back to the copies we made when instanciating the angel
		platBody->body->SetTransform(ogPlatPos, 0);
		deathSensor->body->SetTransform(ogHitPos, 0);
		platBody->body->SetGravityScale(0.0f);
		platBody->body->SetType(b2_staticBody);
		
		if (!cooldwonStated) {
			coolDownTimer.Start();
			cooldwonStated = true;
		}

		deathSensor->body->SetGravityScale(0.0f);
		deathSensor->body->SetType(b2_staticBody);
		attacked = false;
		swordDrop = false;
		attackAnim.Reset();

	}

	currentAnim->Update();

	// Eyes of the statue follow the player
	app->render->DrawTexture(texture, position.x, position.y, false, &currentAnim->GetCurrentFrame());
	SDL_Rect eyeRect = { 6420,2,3,3 };
	if (PlayerX > position.x + (636 / 2)) {
		// if player is to the left move eye sprites to the left
		app->render->DrawTexture(texture,position.x +  eyeL.x + 3,position.y+ eyeL.y, false, &eyeRect);
		app->render->DrawTexture(texture,position.x+  eyeR.x + 3,position.y + eyeR.y, false, &eyeRect);
	}
	else {
		// if player is to the right move eye sprites to the right
		app->render->DrawTexture(texture,position.x + eyeL.x - 3,position.y + eyeL.y, false, &eyeRect);
		app->render->DrawTexture(texture,position.x + eyeR.x - 3,position.y + eyeR.y, false, &eyeRect);
	}

	return true;
}

bool Angel::CleanUp()
{
	return true;
}

void Angel::OnCollision(PhysBody* physA, PhysBody* physB) {


	switch (physB->ctype)
	{
	case ColliderType::ENEMY:
		LOG("Collision ENEMY");

		break;

	case ColliderType::PLAYER:
		LOG("Player touched orb");
		break;


	}
}
void Angel::LoadAnimations() {

	pugi::xml_node myNode = app->scene->scene_parameter.child(this->name.GetString());

	for (pugi::xml_node node = myNode.child("animations").child("attacking").child("frame"); node != NULL; node = node.next_sibling("frame")) {

		attackAnim.PushBack({ node.attribute("x").as_int() , node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() });
		attackAnim.speed = myNode.child("animations").child("attacking").child("speed").attribute("value").as_float() / 16;

	}
	for (pugi::xml_node node = myNode.child("animations").child("rising").child("frame"); node != NULL; node = node.next_sibling("frame")) {

		riseAnim.PushBack({ node.attribute("x").as_int() , node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() });
		riseAnim.speed = myNode.child("animations").child("rising").child("speed").attribute("value").as_float() / 16;

	}
	
}

