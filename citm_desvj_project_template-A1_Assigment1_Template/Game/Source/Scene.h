#ifndef __SCENE_H__
#define __SCENE_H__

#include "Module.h"
#include "Player.h"
#include "../Morgan.h"
#include "../Spear.h"
#include "../Orb.h"
#include "../Jorge.h"

struct SDL_Texture;

class Scene : public Module
{
public:

	Scene();

	// Destructor
	virtual ~Scene();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	void SpawnGoons(bool first = false);

	Player* player;
	Orb* orbs[50] = { nullptr };

	bool SaveState(pugi::xml_node node);

	bool LoadState(pugi::xml_node node);

	pugi::xml_node scene_parameter;

	SDL_Texture* pathTexture = nullptr;

private:
	SDL_Texture* mouseTileTex;
	SDL_Texture* img;

	int currentLvl = 0;

	float textPosX, textPosY = 0;
	uint texW, texH;
	uint windowW, windowH;
	bool isInDebugMode;

public:

	uint enemyDeathEffect;

};

#endif // __SCENE_H__