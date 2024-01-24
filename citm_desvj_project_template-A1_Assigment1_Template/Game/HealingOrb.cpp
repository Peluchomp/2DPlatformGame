#include "HealingOrb.h"
#include "Source/App.h"
#include "Source/Textures.h"
#include "Source/Audio.h"
#include "Source/Input.h"
#include "Source/Render.h"
#include "Source/Scene.h"
#include "Source/Log.h"
#include "Source/Point.h"
#include "Source/Physics.h"

HealingOrb::HealingOrb() : Entity(EntityType::ORB)
{
	name.Create("healingorb");
}

HealingOrb ::~HealingOrb() {}

bool HealingOrb::Awake() {

	// the awake is only called for entities that are awaken with the manager


	return true;
}

bool HealingOrb::Start() {

	//initilize textures

	return true;
}
bool HealingOrb::PreUpdate(float dt)
{

	return  true;
}

bool HealingOrb::Update(float dt)
{


	if (!awake) {
		texturePath = parameters.attribute("texturepath").as_string();
		texture = app->tex->Load(texturePath);

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
		_body->ctype = ColliderType::HEALINGORB;

	}

	if (_body->active) {


		currentAnimation->Update();

		// Blit

		app->render->DrawTexture(texture, position.x, position.y, false, &currentAnimation->GetCurrentFrame());

	}


	return true;
}

bool HealingOrb::CleanUp()
{
	return true;
}

void HealingOrb::OnCollision(PhysBody* physA, PhysBody* physB) {


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

