#include "EntityManager.h"
#include "Player.h"
#include "App.h"
#include "Textures.h"
#include "Scene.h"
#include "../Spear.h"
#include "../Orb.h"
#include "../HealingOrb.h"
#include "../Jorge.h"
#include "../Morgan.h"
#include "../Chandelier.h"
#include "../Angel.h"
#include "../MegaMorgan.h"
#include "../Aelfric.h"
#include "../Checkpoint.h"
#include "../FloorSpears.h"
#include "../EvilSpearLightning.h"
#include "../EvilSpin.h"
#include "../TitleScreen.h"
#include "Optick/include/optick.h"
#include "Defs.h"
#include "Log.h"

EntityManager::EntityManager() : Module()
{
	name.Create("entitymanager");
}

// Destructor
EntityManager::~EntityManager()
{}

// Called before render is available
bool EntityManager::Awake(pugi::xml_node& config)
{
	LOG("Loading Entity Manager");
	bool ret = true;

	myNode = config;

	//Iterates over the entities and calls the Awake
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;

	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		pEntity = item->data;

		if (pEntity->active == false) continue;
		ret = item->data->Awake();
	}

	return ret;

}

bool EntityManager::Start() {

	bool ret = true;

	const char* enemyTexPath = myNode.child("enemyTexture").attribute("path").as_string();
	enemy_tex = app->tex->Load(enemyTexPath);
	chandelier_tex = app->tex->Load(myNode.child("chandelier").attribute("path").as_string());

	// CHANGE
	angel_tex = app->tex->Load(myNode.child("angleTexture").attribute("path").as_string());

	//Iterates over the entities and calls Start
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;

	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		pEntity = item->data;

		if (pEntity->active == false) continue;
		ret = item->data->Start();
	}


	return ret;
}

// Called before quitting
bool EntityManager::CleanUp()
{
	bool ret = true;
	ListItem<Entity*>* item;
	item = entities.end;

	while (item != NULL && ret == true)
	{
		ret = item->data->CleanUp();
		item = item->prev;
	}

	entities.Clear();

	return ret;
}

Entity* EntityManager::CreateEntity(EntityType type)
{
	Entity* entity = nullptr;

	switch (type)
	{
	case EntityType::PLAYER:
		entity = new Player();
		break;
	case EntityType::MORGAN:
		entity = new Morgan();
		break;
	case EntityType::SPEAR:
		entity = new Spear();
		break;
	case EntityType::ORB:
		entity = new Orb();
		entity->active = true;
		break;
	case EntityType::THUNDER:
		entity = new Thunder();
		entity->active = true;
		break;
	case EntityType::JORGE:
		entity = new Jorge();
		break;
	case EntityType::CHANDELIER:
		entity = new Chandelier();
		break;
	case EntityType::ANGEL:
		entity = new Angel();
		break;
	case EntityType::MEGA_MORGAN:
		entity = new MegaMorgan();
		break;
	case EntityType::CHECKPOINT:
		entity = new Checkpoint();
		break;
	case EntityType::AELFRIC:
		entity = new Aelfric();
		break;
	case EntityType::EVILSPEARLIGHTNING:
		entity = new EvilSpearLightning();
		break;
	case EntityType::FLOORSPEAR:
		entity = new FloorSpears();
		break;
	case EntityType::EVILSPIN:
		entity = new EvilSpin();
		break;
		break;
	case EntityType::HEALINGORB:
		entity = new HealingOrb();
		break;
	default:
		break;
	}

	


	entities.Add(entity);
	entity->pathTexture = app->scene->pathTexture;

	return entity;
}

void EntityManager::DestroyEntity(Entity* entity)
{
	ListItem<Entity*>* item;

	for (item = entities.start; item != NULL; item = item->next)
	{
		if (item->data == entity) entities.Del(item);
	}
}

void EntityManager::AddEntity(Entity* entity)
{
	if (entity != nullptr) entities.Add(entity);
}

bool EntityManager::Update(float dt)
{
	OPTICK_EVENT("EntityManager");
	bool ret = true;
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;

		for (item = entities.end; item != NULL && ret == true; item = item->prev)
		{
			pEntity = item->data;

			
			if (pEntity->pendingToDestroy && pEntity->type != EntityType::CHANDELIER) {
				for (ListItem<PhysBody*>* corpse = pEntity->myBodies.start; corpse != NULL; corpse = corpse->next) {

 					app->physics->DestroyObject((PhysBody*)corpse->data);
				}

				
				pEntity->pendingToDestroy = false;
				DestroyEntity(pEntity);
			}
			else if (pEntity->pendingToDestroy && pEntity->type == EntityType::CHANDELIER) {

				for (pugi::xml_node orbNode = app->scene->scene_parameter.child("chandelure"); orbNode; orbNode = orbNode.next_sibling("chandelure")) {
					if (pEntity->num == orbNode.attribute("num").as_int()) {
						Chandelier* orb = (Chandelier*)app->entityManager->CreateEntity(EntityType::CHANDELIER);
						orb->position.x = orbNode.attribute("x").as_int();
						orb->position.y = orbNode.attribute("y").as_int();
						orb->num = orbNode.attribute("num").as_int();
						orb->parameters = orbNode;
						orb->Awake();
						orb->Start();
						orb->breackFx = app->scene->chandelierDeathFx;
					}

				}

				for (ListItem<PhysBody*>* corpse = pEntity->myBodies.start; corpse != NULL; corpse = corpse->next) {

					app->physics->DestroyObject((PhysBody*)corpse->data);
				}
				pEntity->pendingToDestroy = false;
				DestroyEntity(pEntity);
			}
			if (pEntity->active == false ) continue;

			if (app->titleS->options == false || item->data == app->scene->player || item->data == app->scene->player->mySpear)
			ret = item->data->Update(dt);
		}
		for (ListItem<b2RevoluteJoint*>* corpse = destroyJoints.start; corpse != NULL; corpse = corpse->next) {

			

				app->physics->world->DestroyJoint(corpse->data);
			

		}
	
	

	return ret;
}


bool EntityManager::PostUpdate() {
	// This will be used for entities that have to render something overlapping

	bool ret = true;
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;
	for (item = entities.end; item != NULL && ret == true; item = item->prev)
	{
		pEntity = item->data;
		ret = pEntity->PostUpdate();
	}
	return ret;
}

bool EntityManager::LoadState(pugi::xml_node node) {
	OPTICK_EVENT("Load entities")

	ListItem<Entity*>* item;

	bool ret = true;

	Entity* pEntity = NULL;

	DestroyAll(); /*Destroy all entities*/

    pEntity = NULL;


	pugi::xml_node LevelNode = node.child("currentLevel");
	
	int LevelValue = LevelNode.attribute("value").as_int();

	if(LevelValue == 0){
		node = node.child("level0"); 
	}
	else if (LevelValue == 1) {
		node = node.child("level1");
	}

	// Spawn saved entities
	for (item = app->entityManager->savedEntities.start; item != NULL && ret == true; item = item->next)
	{
		pEntity = item->data;

		if (pEntity->type == EntityType::ORB) {

			for (pugi::xml_node orbNode = node.child(pEntity->name.GetString()); orbNode; orbNode = orbNode.next_sibling("orb")) {
				if (pEntity->num == orbNode.attribute("num").as_int()) {
					pEntity = app->entityManager->CreateEntity(EntityType::ORB);
					pEntity->position.x = orbNode.child("position").attribute("x").as_int();
					pEntity->position.y = orbNode.child("position").attribute("y").as_int();
					pEntity->num = orbNode.attribute("num").as_int();
					pEntity->parameters = app->scene->scene_parameter.child("orb");

				}
			}
		}
		if (pEntity->type == EntityType::JORGE) {

			for (pugi::xml_node orbNode = node.child("jorge"); orbNode; orbNode = orbNode.next_sibling("jorge")) {
				if (pEntity->num == orbNode.attribute("num").as_int()) {
					pEntity = app->entityManager->CreateEntity(EntityType::JORGE);
					pEntity->position.x = orbNode.child("position").attribute("x").as_int();
					pEntity->position.y = orbNode.child("position").attribute("y").as_int();
					pEntity->num = orbNode.attribute("num").as_int();
					pEntity->parameters = orbNode;
					pEntity->Start();
		
				}
			}
		}
		else if (pEntity->type == EntityType::MORGAN) {

			for (pugi::xml_node orbNode = node.child("morgan"); orbNode; orbNode = orbNode.next_sibling("morgan")) {
				if (pEntity->num == orbNode.attribute("num").as_int()) {
					pEntity = app->entityManager->CreateEntity(EntityType::MORGAN);
					pEntity->position.x = orbNode.child("position").attribute("x").as_int();
					pEntity->position.y = orbNode.child("position").attribute("y").as_int();
					pEntity->num = orbNode.attribute("num").as_int();
					pEntity->parameters = orbNode;
					pEntity->Start();

				}
			}
		}
		else if (pEntity->type == EntityType::MEGA_MORGAN) {

			for (pugi::xml_node orbNode = node.child("mega_morgan"); orbNode; orbNode = orbNode.next_sibling("mega_morgan")) {
				if (pEntity->num == orbNode.attribute("num").as_int()) {
					pEntity = app->entityManager->CreateEntity(EntityType::MEGA_MORGAN);
					pEntity->position.x = orbNode.child("position").attribute("x").as_int();
					pEntity->position.y = orbNode.child("position").attribute("y").as_int();
					pEntity->num = orbNode.attribute("num").as_int();
					pEntity->parameters = orbNode;
					pEntity->Start();

				}
			}
		}
		else if (pEntity->type == EntityType::CHANDELIER) {

			for (pugi::xml_node orbNode = node.child("chandelure"); orbNode; orbNode = orbNode.next_sibling("chandelure")) {
				if (pEntity->num == orbNode.attribute("num").as_int()) {
					pEntity = app->entityManager->CreateEntity(EntityType::CHANDELIER);

					Chandelier* chand = (Chandelier*)pEntity;

					pEntity->position.x = orbNode.child("position").attribute("x").as_int();
					pEntity->position.y = orbNode.child("position").attribute("y").as_int();
					pEntity->num = orbNode.attribute("num").as_int();
					pEntity->parameters = orbNode;
					pEntity->Awake();
					pEntity->Start();

				}
			}
		}

	}
	// entities.Clear(); this removes them from the list, it doesnt delete them

	return true;
}

// L14: TODO 8: Create a method to save the state of the renderer
// using append_child and append_attribute
bool EntityManager::SaveState(pugi::xml_node node) {

	OPTICK_EVENT("Save entities")
	savedEntities = entities;

	ListItem<Entity*>* item;

	pugi::xml_node LevelNode = node.append_child("currentLevel");
	LevelNode.append_attribute("value").set_value(app->scene->currentLvl);

	bool ret = true;

	Entity* pEntity = NULL;
	if (app->scene->currentLvl == 0) {

		if (node.child("level0") == NULL) {
			node = node.append_child("level0");
		}
		else {
			node = node.child("level0");
		}

		for (item = entities.start; item != NULL && ret == true; item = item->next)
		{
			pEntity = item->data;
			if (pEntity->type == EntityType::MORGAN) {

				pugi::xml_node morganNode = node.append_child(pEntity->name.GetString());
				morganNode.append_attribute("HP").set_value(pEntity->hp);
				morganNode.append_attribute("num").set_value(pEntity->num);
				morganNode = morganNode.append_child("position");
				morganNode.append_attribute("x").set_value(pEntity->position.x);
				morganNode.append_attribute("y").set_value(pEntity->position.y);

			}
			if (pEntity->type == EntityType::JORGE) {

				pugi::xml_node morganNode = node.append_child(pEntity->name.GetString());
				morganNode.append_attribute("HP").set_value(pEntity->hp);
				morganNode.append_attribute("num").set_value(pEntity->num);
				morganNode = morganNode.append_child("position");
				morganNode.append_attribute("x").set_value(pEntity->position.x);
				morganNode.append_attribute("y").set_value(pEntity->position.y);

			}
			if (pEntity->type == EntityType::MEGA_MORGAN) {

				pugi::xml_node morganNode = node.append_child(pEntity->name.GetString());
				morganNode.append_attribute("HP").set_value(pEntity->hp);
				morganNode.append_attribute("num").set_value(pEntity->num);
				morganNode = morganNode.append_child("position");
				morganNode.append_attribute("x").set_value(pEntity->position.x);
				morganNode.append_attribute("y").set_value(pEntity->position.y);

			}
			if (pEntity->type == EntityType::ORB) {

				pugi::xml_node morganNode = node.append_child(pEntity->name.GetString());
				morganNode.append_attribute("num").set_value(pEntity->num);
				morganNode = morganNode.append_child("position");
				morganNode.append_attribute("x").set_value(pEntity->position.x);
				morganNode.append_attribute("y").set_value(pEntity->position.y);

			}
			
		}
	}
	else {

		if (node.child("level1") == NULL) {
			node = node.append_child("level1");
		}
		else {
			node = node.child("level1");
		}

		for (item = entities.start; item != NULL && ret == true; item = item->next)
		{
			pEntity = item->data;
			if (pEntity->type == EntityType::MORGAN) {

				pugi::xml_node morganNode = node.append_child(pEntity->name.GetString());
				morganNode.append_attribute("HP").set_value(pEntity->hp);
				morganNode.append_attribute("num").set_value(pEntity->num);
				morganNode = morganNode.append_child("position");
				morganNode.append_attribute("x").set_value(pEntity->position.x);
				morganNode.append_attribute("y").set_value(pEntity->position.y);

			}
			if (pEntity->type == EntityType::JORGE) {

				pugi::xml_node morganNode = node.append_child(pEntity->name.GetString());
				morganNode.append_attribute("HP").set_value(pEntity->hp);
				morganNode.append_attribute("num").set_value(pEntity->num);
				morganNode = morganNode.append_child("position");
				morganNode.append_attribute("x").set_value(pEntity->position.x);
				morganNode.append_attribute("y").set_value(pEntity->position.y);

			}
			if (pEntity->type == EntityType::MEGA_MORGAN) {

				pugi::xml_node morganNode = node.append_child(pEntity->name.GetString());
				morganNode.append_attribute("HP").set_value(pEntity->hp);
				morganNode.append_attribute("num").set_value(pEntity->num);
				morganNode = morganNode.append_child("position");
				morganNode.append_attribute("x").set_value(pEntity->position.x);
				morganNode.append_attribute("y").set_value(pEntity->position.y);

			}
			if (pEntity->type == EntityType::ORB) {

				pugi::xml_node morganNode = node.append_child(pEntity->name.GetString());
				morganNode.append_attribute("num").set_value(pEntity->num);
				morganNode = morganNode.append_child("position");
				morganNode.append_attribute("x").set_value(pEntity->position.x);
				morganNode.append_attribute("y").set_value(pEntity->position.y);

			}
			if (pEntity->type == EntityType::CHANDELIER) {

				pugi::xml_node morganNode = node.append_child(pEntity->name.GetString());
				morganNode.append_attribute("num").set_value(pEntity->num);
				morganNode = morganNode.append_child("position");
				morganNode.append_attribute("x").set_value(pEntity->position.x);
				morganNode.append_attribute("y").set_value(pEntity->position.y);
				Chandelier* chand = (Chandelier*)pEntity;
				if (chand->myType == 0) { morganNode.append_attribute("type").set_value(0); }
				else if (chand->myType == 1) { morganNode.append_attribute("type").set_value(1); }
			}
		}
	}

	return true;
}

void EntityManager::ReSpawn() {

	ListItem<Entity*>* item;

	Entity* pEntity = NULL;

	// Delete ALL entities
	for (item = entities.start; item != NULL; item = item->next)
	{
		pEntity = item->data;
		// Orbs purposely dont respawn after you collect them
		if ( pEntity->type == EntityType::MORGAN || pEntity->type == EntityType::JORGE || pEntity->type == EntityType::MEGA_MORGAN) {
			for (ListItem<PhysBody*>* corpse = pEntity->myBodies.start; corpse != NULL; corpse = corpse->next) {

				// Destroy all of the entity's b2bodies
				app->physics->DestroyObject((PhysBody*)corpse->data);
			}
			pEntity->pendingToDestroy = false;
			DestroyEntity(pEntity);
		}
	}

	app->scene->SpawnGoons();

}

void EntityManager::DestroyAll() {

	ListItem<Entity*>* item;

	bool ret = true;

	Entity* pEntity = NULL;

	// Delete ALL entities
	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		pEntity = item->data;
		if ((pEntity->type == EntityType::ORB || pEntity->type == EntityType::MORGAN || pEntity->type == EntityType::JORGE || pEntity->type == EntityType::MEGA_MORGAN || pEntity->type == EntityType::CHANDELIER || pEntity->type == EntityType::ANGEL)) {
			for (ListItem<PhysBody*>* corpse = pEntity->myBodies.start; corpse != NULL; corpse = corpse->next) {

				// Destroy all of the entity's b2bodies
				app->physics->DestroyObject((PhysBody*)corpse->data);
			}
			pEntity->pendingToDestroy = false;
			DestroyEntity(pEntity);
		}
	}

}