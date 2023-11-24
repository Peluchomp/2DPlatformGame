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

	texturePath = parameters.attribute("texturepath").as_string();



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

	b2Vec2 vel;

	



	
	return true;
}

bool Orb::CleanUp()
{
	return true;
}

void Orb::OnCollision(PhysBody* physA, PhysBody* physB) {


	switch (physB->ctype)
	{
	case ColliderType::ITEM:
		LOG("Collision ITEM");

		break;
	
	case ColliderType::PLAYER :
		LOG("Player touched orb");
		break;


	}
}

