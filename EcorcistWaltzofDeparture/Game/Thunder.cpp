#include "Thunder.h"
#include "Source/App.h"
#include "Source/Textures.h"
#include "Source/Audio.h"
#include "Source/Input.h"
#include "Source/Render.h"
#include "Source/Scene.h"
#include "Source/Log.h"
#include "Source/Point.h"
#include "Source/Physics.h"

Thunder::Thunder() : Entity(EntityType::THUNDER)
{
	name.Create("thunder");
}

Thunder::~Thunder() {}

bool Thunder::Awake() {

	// the awake is only called for entities that are awaken with the manager



	return true;
}

bool Thunder::Start() {

	//initilize textures



	return true;
}
bool Thunder::PreUpdate(float dt)
{

	return  true;
}

bool Thunder::Update(float dt)
{


	if (!awake) {
		texturePath = parameters.attribute("texturepath").as_string();
		texture = app->tex->Load(texturePath);

		for (pugi::xml_node node = parameters.child("frame"); node != NULL; node = node.next_sibling("frame")) {
			
			defaultAnim.PushBack({ node.attribute("x").as_int() , node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() } , false, node.attribute("audio").as_string());
			defaultAnim.speed = parameters.child("speed").attribute("value").as_float() / 16;
		
			

			defaultAnim.loop = parameters.child("loop").attribute("value").as_bool();
		
		}

		for (pugi::xml_node node = parameters.child("frame2"); node != NULL; node = node.next_sibling("frame2"))
		{

			bossAnim.PushBack({ node.attribute("x").as_int() , node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() }, false, node.attribute("audio").as_string());
			bossAnim.speed = parameters.child("speed").attribute("value").as_float() / 16;
			bossAnim.loop = true;
		}

		awake = true;
		if (bossThunder == false)
		currentAnimation = &defaultAnim;
		else 
		currentAnimation = &bossAnim;

		/*_body = app->physics->CreateCircle(position.x + 19, position.y + 19, 15, bodyType::KINEMATIC, true);
		_body->body->SetGravityScale(0);
		_body->ctype = ColliderType::ORB;*/

	}

	
		currentAnimation->Update();

		// Blit
		if (bossThunder == false)
		app->render->DrawTexture(texture, app->scene->player->position.x -32, app->scene->player->position.y -325, false, &currentAnimation->GetCurrentFrame());
		else 
		app->render->DrawTexture(texture, bossPos.x, bossPos.y , false, &currentAnimation->GetCurrentFrame());
	
	return true;
}

bool Thunder::CleanUp()
{
	return true;
}

void Thunder::OnCollision(PhysBody* physA, PhysBody* physB) {


}
