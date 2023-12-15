#include "EntityManager.h"
#include "Player.h"
#include "App.h"
#include "Textures.h"
#include "Scene.h"
#include "../Spear.h"
#include "../Orb.h"
#include "../Jorge.h"

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
	case EntityType::ITEM:
		entity = new Item();
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
	default:
		break;
	}

	entities.Add(entity);

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
	if ( entity != nullptr) entities.Add(entity);
}

bool EntityManager::Update(float dt)
{
	bool ret = true;
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;
	
	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		pEntity = item->data;

		if (pEntity->pendingToDestroy) {

			for (ListItem<PhysBody*>* corpse = pEntity->myBodies.start; corpse != NULL; corpse = corpse->next) {

				app->physics->DestroyObject((PhysBody*)corpse->data);
				pEntity->pendingToDestroy = false;
				DestroyEntity(pEntity);
			}
		}

		if (pEntity->active == false) continue;
		ret = item->data->Update(dt);
	}

	return ret;
}

bool EntityManager::LoadState(pugi::xml_node node) {

	ListItem<Entity*>* item;
	
	bool ret = true;

	Entity* pEntity = NULL;

	// Delete ALL entities
	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		pEntity = item->data;
		if (pEntity->type == EntityType::ORB) {
			for (ListItem<PhysBody*>* corpse = pEntity->myBodies.start; corpse != NULL; corpse = corpse->next) {

				app->physics->DestroyObject((PhysBody*)corpse->data);
				pEntity->pendingToDestroy = false;
				DestroyEntity(pEntity);
			}
		}
	}
	// entities.Clear(); this removes them from the list, it doesnt delete them

	return true;
}

// L14: TODO 8: Create a method to save the state of the renderer
// using append_child and append_attribute
bool EntityManager::SaveState(pugi::xml_node node) {


	savedEntities = entities;

	return true;
}