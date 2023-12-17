#include "Morgan.h"
#include "Source/App.h"
#include "Source/Textures.h"
#include "Source/Audio.h"
#include "Source/Input.h"
#include "Source/Render.h"
#include "Source/Scene.h"
#include "Source/Log.h"
#include "Source/Point.h"
#include "Source/Physics.h"

Morgan::Morgan() : Entity(EntityType::MORGAN)
{
	name.Create("morgan");
}

Morgan::~Morgan() {}

bool Morgan::Awake() {

	// the awake is only called for entities that are awaken with the manager
	return true;
}

bool SameRectM(SDL_Rect r1, SDL_Rect r2) {
	if (r1.x == r2.x && r1.y == r2.y && r1.w == r2.w && r1.h == r2.h) {
		return true;
	}
	else { return false; }
}

bool Morgan::Start() {

	//initilize textures
	position.x = parameters.child("position").attribute("x").as_int();
	position.y = parameters.child("position").attribute("y").as_int();

	texture = app->entityManager->enemy_tex; // previously loaded from the config by the entity manager
	
	/*the pathTexture is given with the entity's creation*/
	pbody = app->physics->CreateCircle(position.x, position.y, 16, bodyType::DYNAMIC, ColliderType::ENEMY);
	pbody->ctype = ColliderType::ENEMY;
	pbody->body->SetGravityScale(1);

	snakeBody = app->physics->CreateRectangleSensor(position.x, position.y, 51, 13, bodyType::DYNAMIC, ColliderType::ENEMY_ATTACK);
	snakeBody->ctype = ColliderType::ENEMY_ATTACK;
	snakeBody->body->SetGravityScale(0);
	snakeBody->listener = app->scene->player;

	myBodies.Add(pbody);
	myBodies.Add(snakeBody);

	LoadAnimations();
	hp = MORGAN_HP;


	return true;
}


bool Morgan::Update(float dt)
{
	if (pbody != nullptr) {
		if (pbody->body->GetLinearVelocity().x > 0) {
			myDir = Direction::RIGHT;
		}
		else if (pbody->body->GetLinearVelocity().x < 0) {
			myDir = Direction::LEFT;
			
		}

		// L07 DONE 4: Add a physics to an item - update the position of the object from the physics.  
		position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x);
		position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y);

		iPoint origin = iPoint(10, 136);
		iPoint destination = iPoint(20, 134);

		iPoint enemyPos = app->map->WorldToMap(position.x, position.y);
		iPoint playerPos = app->map->WorldToMap(app->scene->player->position.x, app->scene->player->position.y);

		app->map->pathfinding->CreatePath(enemyPos, playerPos);

		const DynArray<iPoint>* path = app->map->pathfinding->GetLastPath();

		for (uint i = 0; i < path->Count(); ++i)
		{
			iPoint pos = app->map->MapToWorld(path->At(i)->x, path->At(i)->y);
			app->render->DrawTexture(pathTexture, pos.x, pos.y, false);
		}

		if (path->Count() > 1 && app->map->pathfinding->CreatePath(enemyPos, playerPos) != -1) {
			iPoint pos = app->map->MapToWorld(path->At(1)->x, path->At(1)->y);


			if (enemyPos.x - playerPos.x < 0 && abs(enemyPos.x - playerPos.x) > 2)
				pbody->body->SetLinearVelocity(b2Vec2(1, 9.8f));
			else if (abs(enemyPos.x - playerPos.x) > 2)
				pbody->body->SetLinearVelocity(b2Vec2(-1, 9.8f));
			else if (abs(enemyPos.x - playerPos.x) < 2) {

				//aqui codigo de atacar
				currentAnimation = &attacking;
				

			}
			else { 
				currentAnimation = &walking; 
				snakeBody->active = false;
			
			}


			timer = 0;
		}
		if (currentAnimation == &attacking) {
			HitBoxManagement();

			pbody->body->SetLinearVelocity(b2Vec2(0, 9.8f));
			pbody->body->SetLinearDamping(0);

			if (myDir == Direction::RIGHT) {
				snakeBody->body->SetTransform(pbody->body->GetPosition() + b2Vec2(0.7f, -0.5f), 0.0f);
			}
			else if (myDir == Direction::LEFT) {
				snakeBody->body->SetTransform(pbody->body->GetPosition() + b2Vec2(-0.7f, -0.5f), 0.0f);
			}
		}

		if (app->scene->player->attackTrigger->Contains(position.x, position.y) || app->scene->player->attackTrigger->Contains(position.x + 32, position.y) || app->scene->player->attackTrigger->Contains(position.x, position.y + 32) || app->scene->player->attackTrigger->Contains(position.x + 32, position.y + 32)) {
			if (app->scene->player->Attacking == true)
				hp--;
		}

		if (hp <= 0) {
			for (ListItem<PhysBody*>* corpse = myBodies.start; corpse != NULL; corpse = corpse->next) {

				// Destroy all of the entity's b2bodies
				app->physics->DestroyObject((PhysBody*)corpse->data);
			}
			pendingToDestroy = false;
			app->entityManager->DestroyEntity(this);
		}

		if (app->map->pathfinding->CreatePath(enemyPos, playerPos) == -1) {

			pbody->body->SetLinearVelocity(b2Vec2(0, 9.8f));
			pbody->body->SetLinearDamping(0);
		}

		position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x);
		position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y);

		SDL_Rect section = { 1,1,99,78 };


		currentAnimation->Update();
		if (myDir == Direction::RIGHT) { app->render->DrawTexture(texture, position.x -20 , position.y - 60, true, &currentAnimation->GetCurrentFrame()); }
		else if (myDir == Direction::LEFT ){ app->render->DrawTexture(texture, position.x - 80, position.y - 60, false, &currentAnimation->GetCurrentFrame()); }

		return true;
	}
}

bool Morgan::CleanUp()
{
	return true;
}

void Morgan::LoadAnimations() {

	pugi::xml_node myNode = app->scene->scene_parameter.child(this->name.GetString());

	for (pugi::xml_node node = myNode.child("animations").child("walking").child("frame"); node != NULL; node = node.next_sibling("frame")) {

		walking.PushBack({ node.attribute("x").as_int() , node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() });
		walking.speed = myNode.child("animations").child("walking").child("speed").attribute("value").as_float() / 16;

	}
	for (pugi::xml_node node = myNode.child("animations").child("attacking").child("frame"); node != NULL; node = node.next_sibling("frame")) {

		attacking.PushBack({ node.attribute("x").as_int() , node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() });
		attacking.speed = myNode.child("animations").child("attacking").child("speed").attribute("value").as_float() / 16;

	}

		   attacking.speed = 0.18016f / 16;

		  
	currentAnimation = &walking;

}

void Morgan::HitBoxManagement() {

	if (currentAnimation == &attacking) {
		snakeBody->active = true;
		    SDL_Rect deactive1 = { 1   ,80, 99 ,78 };
			SDL_Rect deactive2 = { 101 ,80 ,99 ,78 };
			SDL_Rect deactive3 = { 201 ,80 ,99 ,78 };
			SDL_Rect deactive4 = { 301 ,80 ,99 ,78 };

			if (SameRectM(deactive1, currentAnimation->GetCurrentFrame()) || SameRectM(deactive2, currentAnimation->GetCurrentFrame()) || SameRectM(deactive3, currentAnimation->GetCurrentFrame()) || SameRectM(deactive4, currentAnimation->GetCurrentFrame())) {

				snakeBody->body->SetTransform(pbody->body->GetPosition() + b2Vec2(10000, 10000), 0.0f);

			}


	}

}