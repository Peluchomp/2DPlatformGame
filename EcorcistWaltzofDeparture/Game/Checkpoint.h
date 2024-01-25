#ifndef __Checkpoint_H__
#define __Checkpoint_H__

#include "Source/Entity.h"
#include "Source/Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"

struct SDL_Texture;

class Checkpoint : public Entity
{
public:

	Checkpoint();
	virtual ~Checkpoint();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	bool PreUpdate(float dt);

	void OnCollision(PhysBody* physA, PhysBody* physB);

	Animation* currentAnimation;
	Animation defaultAnim;
	Animation saveAnim;
	bool saved = false;

	SDL_Texture* texture = nullptr;

	PhysBody* _body;

	bool awake = false;

private:
	const char* texturePath;



};

#endif // __Checkpoint_H__