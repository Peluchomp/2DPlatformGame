#include "MegaMorgan.h"
#include "Source/App.h"
#include "Source/Textures.h"
#include "Source/Audio.h"
#include "Source/Input.h"
#include "Source/Render.h"
#include "Source/Scene.h"
#include "Source/Log.h"
#include "Source/Point.h"
#include "Source/Physics.h"

MegaMorgan::MegaMorgan() : Entity(EntityType::MEGA_MORGAN)
{
	name.Create("mega_morgan");
}

MegaMorgan::~MegaMorgan() {}

bool MegaMorgan::Awake() {

	// the awake is only called for entities that are awaken with the manager
	return true;
}



bool MegaMorgan::Start() {

	texture2 = app->entityManager->chandelier_tex;
	texture = app->entityManager->enemy_tex; // previously loaded from the config by the entity manager

	/*the pathTexture is given with the entity's creation*/
	pbody = app->physics->CreateCircle(position.x, position.y, 16, bodyType::DYNAMIC, ColliderType::ENEMY);
	pbody->ctype = ColliderType::ENEMY;
	pbody->body->SetGravityScale(1);

	AttackBody = app->physics->CreateCircle(position.x - 20, position.y - 20, 15, bodyType::DYNAMIC, ColliderType::PHYSIC_OBJ);
	AttackBody->ctype = ColliderType::ENEMY_ATTACK;
	AttackBody->body->SetGravityScale(0);
	AttackBody->body->SetSleepingAllowed(false);
	myBodies.Add(AttackBody);

	revol = app->physics->CreateRevolutionJoint(AttackBody, pbody, 2);
	

	myBodies.Add(pbody);
	LoadAnimations();
	hp = 50;
	enemyDeathFx = app->scene->enemyDeathEffect;

	return true;
}


bool MegaMorgan::Update(float dt)
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

		if (position.DistanceTo(app->scene->player->position) < 500) {
			app->map->pathfinding->CreatePath(enemyPos, playerPos);

			const DynArray<iPoint>* path = app->map->pathfinding->GetLastPath();

			if (app->physics->debug) {
				for (uint i = 0; i < path->Count(); ++i)
				{
					iPoint pos = app->map->MapToWorld(path->At(i)->x, path->At(i)->y);
					if (app->physics->debug) app->render->DrawTexture(pathTexture, pos.x, pos.y, false);
				}

			}

			if (path->Count() > 1 && app->map->pathfinding->CreatePath(enemyPos, playerPos) != -1 && path->Count() < 12) {
				iPoint pos = app->map->MapToWorld(path->At(1)->x, path->At(1)->y);


				if (enemyPos.x - playerPos.x < 0 && abs(enemyPos.x - playerPos.x) > 2)
					pbody->body->SetLinearVelocity(b2Vec2(1, 9.8f));
				else if (abs(enemyPos.x - playerPos.x) > 2)
					pbody->body->SetLinearVelocity(b2Vec2(-1, 9.8f));
				


				timer = 0;
			}
			else {
				pbody->body->SetLinearVelocity(b2Vec2(0, 9.8f));
				pbody->body->SetLinearDamping(0);
			}

			if (app->map->pathfinding->CreatePath(enemyPos, playerPos) == -1) {

				pbody->body->SetLinearVelocity(b2Vec2(0, 9.8f));
				pbody->body->SetLinearDamping(0);
			}
		}


		

		if (app->scene->player->attackTrigger->Contains(position.x, position.y) || app->scene->player->attackTrigger->Contains(position.x + 32, position.y) || app->scene->player->attackTrigger->Contains(position.x, position.y + 32) || app->scene->player->attackTrigger->Contains(position.x + 32, position.y + 32)) {
			if (app->scene->player->Attacking == true)
				hp--;
		}
		if (app->scene->player->op_attackTrigger->Contains(position.x, position.y) || app->scene->player->op_attackTrigger->Contains(position.x + 32, position.y) || app->scene->player->op_attackTrigger->Contains(position.x, position.y + 32) || app->scene->player->op_attackTrigger->Contains(position.x + 32, position.y + 32)) {
			if (app->scene->player->Attacking == true)
				hp--;
		}


		if (app->scene->player->mySpear->pbody->Contains(position.x, position.y) || app->scene->player->mySpear->pbody->Contains(position.x + 32, position.y) || app->scene->player->mySpear->pbody->Contains(position.x, position.y + 32) || app->scene->player->mySpear->pbody->Contains(position.x + 32, position.y + 32)) {

			hp--;
		}


		if (hp <= 0) {
			app->audio->PlayFx(enemyDeathFx);
			hp = 100000;
			for (ListItem<PhysBody*>* corpse = myBodies.start; corpse != NULL; corpse = corpse->next) {

				// Destroy all of the entity's b2bodies
				app->physics->DestroyObject((PhysBody*)corpse->data);
			}
			pendingToDestroy = false;
			app->entityManager->DestroyEntity(this);
		}



		position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x);
		position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y);

		SDL_Rect section = { 1,1,99,78 };
		int jX, jY; SDL_Rect r = { 801,307,31,30 };
		AttackBody->GetPosition(jX, jY);
		int mX, mY;
		pbody->GetPosition(mX, mY);
		
		if (app->scene->noir == false) {
			app->render->DrawTexture(texture, jX - 5, jY - 10, false, &r, 255, 1,255,255,255, AttackBody->GetRotation());

			for (int i = 0; i < 10; ++i) {

				b2Vec2 current = b2Vec2(jX + (mX - jX) * i / 10, jY + (mY - jY) * i / 10);
				app->render->DrawTexture(texture2, current.x, current.y, false, &_chain1);


			}

			currentAnimation->Update();
			if (myDir == Direction::RIGHT) { app->render->DrawTexture(texture, position.x - 20, position.y - 60, true, &currentAnimation->GetCurrentFrame()); }
			else if (myDir == Direction::LEFT) { app->render->DrawTexture(texture, position.x - 80, position.y - 60, false, &currentAnimation->GetCurrentFrame()); }
		}
		else{
		
			app->render->DrawTexture(texture, jX - 5, jY - 10, false, &r, 255, 1,0,0,0, AttackBody->GetRotation());

			for (int i = 0; i < 10; ++i) {

				b2Vec2 current = b2Vec2(jX + (mX - jX) * i / 10, jY + (mY - jY) * i / 10);
				app->render->DrawTexture(texture2, current.x, current.y, false, &_chain1, 255, 1, 0, 0, 0);


			}

			currentAnimation->Update();
			if (myDir == Direction::RIGHT) { app->render->DrawTexture(texture, position.x - 20, position.y - 60, true, &currentAnimation->GetCurrentFrame(), 255, 1, 0, 0, 0); }
			else if (myDir == Direction::LEFT) { app->render->DrawTexture(texture, position.x - 80, position.y - 60, false, &currentAnimation->GetCurrentFrame(), 255, 1, 0, 0, 0); }
		
		}


		

		revol->SetMotorSpeed(60);
		return true;
	}


}

bool MegaMorgan::CleanUp()
{
	return true;
}

void MegaMorgan::LoadAnimations() {

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

