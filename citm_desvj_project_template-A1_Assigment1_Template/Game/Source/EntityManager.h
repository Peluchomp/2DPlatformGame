#ifndef __ENTITYMANAGER_H__
#define __ENTITYMANAGER_H__

#include "Module.h"
#include "Entity.h"
#include "List.h"
#include "Box2D/Box2D/Box2D.h"


struct EntityData {

	bool active = true;
	Entity& myEntity;

};

class EntityManager : public Module
{
public:

	EntityManager();

	// Destructor
	virtual ~EntityManager();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called after Awake
	bool Start();

	// Called every frame
	bool Update(float dt);

	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	void ReSpawn();

	void DestroyAll();

	// Additional methods
	Entity* CreateEntity(EntityType type);

	void DestroyEntity(Entity* entity);

	void AddEntity(Entity* entity);

	bool EntityManager::LoadState(pugi::xml_node node);

	bool EntityManager::SaveState(pugi::xml_node node);


public:

	SDL_Texture* enemy_tex = nullptr;
	SDL_Texture* chandelier_tex = nullptr;
	SDL_Texture* angel_tex = nullptr;

	pugi::xml_node myNode;

	PhysBody* shadowZone;

	List<Entity*> entities;

	List<b2RevoluteJoint*> destroyJoints;

	List<Entity*> savedEntities;

};

#endif // __ENTITYMANAGER_H__
