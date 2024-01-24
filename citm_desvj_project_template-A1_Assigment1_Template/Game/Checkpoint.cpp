#include "Checkpoint.h"
#include "Source/App.h"
#include "Source/Textures.h"
#include "Source/Audio.h"
#include "Source/Input.h"
#include "Source/Render.h"
#include "Source/Scene.h"
#include "Source/Log.h"
#include "Source/Point.h"
#include "Source/Physics.h"

Checkpoint::Checkpoint() : Entity(EntityType::CHECKPOINT)
{
	name.Create("checkpoint");
}

Checkpoint::~Checkpoint() {}

bool Checkpoint::Awake() {

	// the awake is only called for entities that are awaken with the manager


	return true;
}

bool Checkpoint::Start() {

	//initilize textures

	return true;
}
bool Checkpoint::PreUpdate(float dt)
{

	return  true;
}

bool Checkpoint::Update(float dt)
{


	if (!awake) {
		texturePath = parameters.attribute("texturepath").as_string();
		texture = app->tex->Load(texturePath);

		for (pugi::xml_node node = parameters.child("animations").child("idle").child("frame"); node != NULL; node = node.next_sibling("frame")) {

			defaultAnim.PushBack({ node.attribute("x").as_int() , node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() });
			defaultAnim.speed = parameters.child("animations").child("idle").child("speed").attribute("value").as_float() / 16;
			defaultAnim.loop = parameters.child("animations").child("idle").child("loop").attribute("value").as_bool();
			
		}
		for (pugi::xml_node node = parameters.child("animations").child("checked").child("frame"); node != NULL; node = node.next_sibling("frame")) {

			saveAnim.PushBack({ node.attribute("x").as_int() , node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() });
			saveAnim.speed = parameters.child("animations").child("checked").child("speed").attribute("value").as_float() / 16;
			saveAnim.loop = parameters.child("animations").child("checked").child("loop").attribute("value").as_bool();

		}
		awake = true;
		currentAnimation = &defaultAnim;
		_body = app->physics->CreateRectangle(position.x +40, position.y+60, 78,120, bodyType::KINEMATIC, ColliderType::PLATFORM, true);
		myBodies.Add(_body);
		_body->listener = this;
		_body->body->SetGravityScale(0);
		_body->body->GetFixtureList()->SetSensor(true);
		_body->ctype = ColliderType::CHECKPOINT;

	}

	if (_body->active) {





		currentAnimation->Update();

		// Blit

		app->render->DrawTexture(texture, position.x, position.y, false, &currentAnimation->GetCurrentFrame());

	}


	return true;
}

bool Checkpoint::CleanUp()
{
	return true;
}

void Checkpoint::OnCollision(PhysBody* physA, PhysBody* physB) {


	switch (physB->ctype)
	{
	case ColliderType::ENEMY:
		LOG("Collision ENEMY");

		break;

	case ColliderType::PLAYER:
		LOG("Player touched Checkpoint");
		if (!saved) {
			currentAnimation = &saveAnim;
		}
		break;


	}
}

