#include "Jorge.h"
#include "../App.h"
#include "../Textures.h"
#include "../Audio.h"
#include "../Input.h"
#include "../Render.h"
#include "../Scene.h"
#include "../Log.h"
#include "../Point.h"
#include "../Physics.h"
#include "../Window.h"
#include "../Player.h"
Jorge::Jorge() : Entity(EntityType::JORGE)
{
	name.Create("jorge");
}

Jorge::~Jorge() {}

bool Jorge::Awake() {

	

	return true;
}

bool Jorge::Start() {

	//initilize textures
	position.x = parameters.child("position").attribute("x").as_int();
	position.y = parameters.child("position").attribute("y").as_int();
	
	texture = app->entityManager->enemy_tex;

	Bubble = app->physics->CreateCircle(position.x + 16, position.y + 16,10, bodyType::DYNAMIC, ColliderType::ENEMY_ATTACK);
	Bubble->ctype = ColliderType::ENEMY_ATTACK;
	Bubble->body->SetGravityScale(0);
	Bubble->body->GetFixtureList()->SetSensor(true);
	b2Vec2 positiondissapera = b2Vec2(100, 100);
	Bubble->body->SetTransform(positiondissapera, 0);

	pbody = app->physics->CreateCircle(position.x, position.y, 16, bodyType::DYNAMIC, ColliderType::ENEMY);
	pbody->ctype = ColliderType::ENEMY;
	pbody->body->SetGravityScale(0);

	myBodies.Add(Bubble);
	myBodies.Add(pbody);

	pugi::xml_node myNode = app->scene->scene_parameter.child(this->name.GetString());
	for (pugi::xml_node node = myNode.child("swimming").child("frame"); node != NULL; node = node.next_sibling("frame")) {

		swimming.PushBack({ node.attribute("x").as_int() , node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() });
		swimming.speed = parameters.child("swimming").child("speed").attribute("value").as_float() / 16;
	
	}
	for (pugi::xml_node node = myNode.child("attacking").child("frame"); node != NULL; node = node.next_sibling("frame")) {

		attacking.PushBack({ node.attribute("x").as_int() , node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() },false, node.attribute("audio").as_string());
		attacking.speed = parameters.child("attacking").child("speed").attribute("value").as_float() / 16;
		attacking.loop = false;
	}
	currentAnimation = &swimming;
	hp = JORGE_HP;

	enemyDeathFx = app->scene->enemyDeathEffect;

	return true;
}

bool Jorge::Update(float dt)
{
	// L07 DONE 4: Add a physics to an item - update the position of the object from the physics.  
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x);
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y);

	fposition.x = METERS_TO_PIXELS(Bubble->body->GetTransform().p.x);
	fposition.y = METERS_TO_PIXELS(Bubble->body->GetTransform().p.y);

	iPoint origin = iPoint(10, 136);
	iPoint destination = iPoint(20, 134);

	iPoint enemyPos = app->map->WorldToMap(position.x, position.y);
	iPoint playerPos = app->map->WorldToMap(app->scene->player->position.x, app->scene->player->position.y);

	if (position.DistanceTo(app->scene->player->position) < 500) {
		app->map->pathfinding->CreatePath(enemyPos, playerPos);

		const DynArray<iPoint>* path = app->map->pathfinding->GetLastPath();
		b2Vec2 posi;
		if (app->physics->debug) {
			for (uint i = 0; i < path->Count(); ++i)
			{
				iPoint pos = app->map->MapToWorld(path->At(i)->x, path->At(i)->y);
				//app->render->DrawTexture(texture, pos.x, pos.y, false);

			}
		}
		if (path->Count() > 2 && app->map->pathfinding->CreatePath(enemyPos, playerPos) != -1 && path->Count() < 10) {

			iPoint pos = app->map->MapToWorld(path->At(2)->x, path->At(2)->y);

			float dirx = position.x - pos.x;
			float diry = position.y - pos.y;

			b2Vec2 vel;
			vel.x = dirx;
			vel.y = diry;
			vel.Normalize();

			pbody->body->SetLinearVelocity(b2Vec2(-vel.x * 2, -vel.y * 2));

			if (abs(enemyPos.x - playerPos.x) < 2) {

				//aqui codigo de atacar
				if (timer > 60)
				{
					currentAnimation = &attacking;

				}
				if (attacking.HasFinished()) {
					attacking.Reset();
					currentAnimation = &swimming;
					b2Vec2 vel;
					vel.x = app->scene->player->position.x + 15 - position.x;
					vel.y = app->scene->player->position.y - position.y;
					vel.Normalize();
					vel.x *= dt / 2;
					vel.y *= dt / 2;


					Bubble->body->SetTransform(pbody->body->GetPosition(), 0);
					Bubble->body->SetLinearVelocity(vel);
				
					timer = 0;
				}
				pbody->body->SetLinearVelocity(b2Vec2(0, 0));
				pbody->body->SetLinearDamping(0);
			}


		}
		else
		{
			pbody->body->SetLinearVelocity(b2Vec2(0, 0));
			pbody->body->SetLinearDamping(0);
		}
	}
		

		
	timer++;
	if (app->scene->player->attackTrigger->Contains(position.x, position.y) || app->scene->player->attackTrigger->Contains(position.x + 32, position.y) || app->scene->player->attackTrigger->Contains(position.x, position.y + 32) || app->scene->player->attackTrigger->Contains(position.x + 32, position.y + 32)) {
		if (app->scene->player->Attacking == true)
			hp--;
	}

	if (app->scene->player->mySpear->pbody->Contains(position.x, position.y) || app->scene->player->mySpear->pbody->Contains(position.x + 32, position.y) || app->scene->player->mySpear->pbody->Contains(position.x, position.y + 32) || app->scene->player->mySpear->pbody->Contains(position.x + 32, position.y + 32)) {
	
			hp--;
	}


	if (app->scene->player->attackTrigger->Contains(fposition.x, fposition.y) || app->scene->player->attackTrigger->Contains(fposition.x + 16, fposition.y) || app->scene->player->attackTrigger->Contains(fposition.x, fposition.y + 16) || app->scene->player->attackTrigger->Contains(fposition.x + 16, fposition.y + 16)) {
		if (app->scene->player->Attacking == true) 
		{
			b2Vec2 positiondissapera = b2Vec2(100, 100);
			Bubble->body->SetTransform(positiondissapera, 0);
			Bubble->body->SetLinearVelocity(b2Vec2(0,0));
		}
			
	}

	if (app->scene->player->mySpear->pbody->Contains(fposition.x, fposition.y) || app->scene->player->mySpear->pbody->Contains(fposition.x + 16, fposition.y) || app->scene->player->mySpear->pbody->Contains(fposition.x, fposition.y + 16) || app->scene->player->mySpear->pbody->Contains(fposition.x + 16, fposition.y + 16)) {
		
			b2Vec2 positiondissapera = b2Vec2(100, 100);
			Bubble->body->SetTransform(positiondissapera, 0);
			Bubble->body->SetLinearVelocity(b2Vec2(0, 0));
		

	}

	int bubbleX; int bubbleY;
	Bubble->GetPosition(bubbleX, bubbleY);
	SDL_Rect bbRect = { 1026, 130, 15, 15 };
	app->render->DrawTexture(texture, bubbleX, bubbleY, false, &bbRect); /*not sure where to draw the bubble*/

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


	currentAnimation->Update();
	app->render->DrawTexture(texture, position.x-25, position.y-30, false, &currentAnimation->GetCurrentFrame());
	SDL_Rect section = { 1,1,99,78 };
	//app->render->DrawTexture(pathTexture, position.x - 64, position.y - 64, false, &section);



	return true;
}

bool Jorge::CleanUp()
{
	return true;
}