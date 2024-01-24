#ifndef __SCENE_H__
#define __SCENE_H__

#include "Module.h"
#include "Player.h"
#include "../Morgan.h"
#include "../Spear.h"
#include "../Orb.h"
#include "../Jorge.h"
#include "../MegaMorgan.h"
#include "GuiControl.h"
#include "GuiControlButton.h"
#include <string> 

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

	bool OnGuiMouseClickEvent(GuiControl* control);

	pugi::xml_node scene_parameter;

	SDL_Texture* pathTexture = nullptr;

private:
	SDL_Texture* mouseTileTex;
	SDL_Texture* img;

	// Entity Textures

	

	float textPosX, textPosY = 0;
	uint texW, texH;

	bool isInDebugMode;
	GuiControlButton* gcButtom;
	GuiControlButton* musicButtom;
	GuiControlButton* fullScreenButtom;
	GuiControlButton* vSyncButtom;
	GuiControlButton* resumeButtom;
	GuiControlButton* backtittleButtom;

public:

	// Detect when the player reaches the shadow part of the second level
	PhysBody* blackDetection;
	bool noir = false;
	uint windowW, windowH;
	// Locl the boss room when the player arrives
	PhysBody* bossDoor;
	bool bossZone = false;

	SDL_Texture* itemTexture;

	int currentLvl = 0;
	int prevLevel = 0;

	uint enemyDeathEffect;
	uint chandelierDeathFx;
	bool exit;
	float volume = 90;
	bool fullscreen = false;
	bool vSync = false;
	int fullscreenOnce;
	int vsyncOnce;
	std::string scoreText;
	int score;
};

#endif // __SCENE_H__