#ifndef __MODULEENEMIES_H__
#define __MODULEENEMIES_H__

#include "Source/Module.h"
#include "Source/Entity.h"
#include "Source/List.h"
#include "Source/EntityManager.h"

class ModuleEnemies : public Module
{
public:

	ModuleEnemies();

	// Destructor
	virtual ~ModuleEnemies();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called after Awake
	bool Start(pugi::xml_node& conf);

	// Called every frame
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();


public:



};

#endif // __ENTITYMANAGER_H__
