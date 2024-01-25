#include "Orb.h"
#include "Source/App.h"
#include "Source/Textures.h"
#include "Source/Audio.h"
#include "Source/Input.h"
#include "Source/Render.h"
#include "Source/Scene.h"
#include "Source/Log.h"
#include "Source/Point.h"
#include "Source/Physics.h"

Orb::Orb() : Entity(EntityType::ORB)
{
	name.Create("orb");
}

Orb::~Orb() {}

bool Orb::Awake() {

	// the awake is only called for entities that are awaken with the manager


	return true;
}

bool Orb::Start() {

	//initilize textures

	return true;
}
bool Orb::PreUpdate(float dt)
{

	return  true;
}

bool Orb::Update(float dt)
{


	if (!awake) {
		
		texture = app->scene->itemTexture;

		for (pugi::xml_node node = parameters.child("frame"); node != NULL; node = node.next_sibling("frame")) {

			defaultAnim.PushBack({ node.attribute("x").as_int() , node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() });
			defaultAnim.speed = parameters.child("speed").attribute("value").as_float() / 16;
			//idle.loop = parameters.child("animations").child("idle").child("loop").attribute("value").as_bool();
			/*position = app->scene->player->position;
			position.y -= 100;*/
		}
		awake = true;
		currentAnimation = &defaultAnim;
		_body = app->physics->CreateCircle(position.x + 19, position.y + 19, 15, bodyType::KINEMATIC, ColliderType::PLATFORM, true);
		myBodies.Add(_body);
		
		_body->listener = this;
		_body->body->SetGravityScale(0);
		_body->ctype = ColliderType::ORB;

	}

	if (_body->active) {





		currentAnimation->Update();

		// Blit

		app->render->DrawTexture(texture, position.x, position.y, false, &currentAnimation->GetCurrentFrame());

	}


	return true;
}

bool Orb::CleanUp()
{
	return true;
}

void Orb::OnCollision(PhysBody* physA, PhysBody* physB) {


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

