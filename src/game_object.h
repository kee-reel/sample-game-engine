#ifndef H_GAME_OBJECT_
#define H_GAME_OBJECT_
#include "includes.h"

#include "transform.h"

class GameObject
{
public:
    GameObject() {}

	virtual ~GameObject() {}

public:
	Transform transform;
};
#endif
