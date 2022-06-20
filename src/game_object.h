#ifndef H_GAME_OBJECT_
#define H_GAME_OBJECT_
#include "includes.h"

#include "transform.h"
#include "component.h"


class GameObject : public Component
{
public:
    GameObject() :
        Component(Component::GAME_OBJECT)
    {}

	virtual ~GameObject() {}

public:
	Transform transform;
};
#endif
