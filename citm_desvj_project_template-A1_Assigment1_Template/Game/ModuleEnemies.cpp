#include "ModuleEnemies.h"
#include "Source/Player.h"
#include "Source/App.h"
#include "Source/Textures.h"
#include "Source/Scene.h"
#include "Source/EntityManager.h"

#include "Source/Defs.h"
#include "Source/Log.h"

ModuleEnemies::ModuleEnemies() : Module()
{
	name.Create("moduleEnemies");
}

// Destructor
ModuleEnemies::~ModuleEnemies()
{}

// Called before render is available
bool ModuleEnemies::Awake(pugi::xml_node& config)
{
	LOG("Loading Entity Manager");
	bool ret = true;
	for (pugi::xml_node itemNode = config.child("item"); itemNode; itemNode = itemNode.next_sibling("item"))
	{
		//Item* item = (Item*)app->entityManager->CreateEntity(EntityType::ITEM);
		//item->parameters = itemNode;
	}

	return ret;

}

bool ModuleEnemies::Start() {

	bool ret = true;


	//Iterates over the entities and calls Start

	return ret;
}

// Called before quitting
bool ModuleEnemies::CleanUp()
{
	bool ret = true;

	return ret;
}

bool ModuleEnemies::Update(float dt)
{
	bool ret = true;


	
	return ret;
}