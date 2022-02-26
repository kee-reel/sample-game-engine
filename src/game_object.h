#include "includes.h"

#include "transform.h"
#include "component.h"
#include "sge.h"

class GameObject : public Component, public sge::IGameObject
{
public:
    GameObject() :
        Component(Component::GAME_OBJECT)
    {}

	virtual ~GameObject() {}

	sge::ITransform &get_transform() { return transform; }

public:
	Transform transform;
};
