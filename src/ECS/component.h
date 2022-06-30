#ifndef ECS_COMPONENT_
#define ECS_COMPONENT_

#include "../includes.h"

namespace ecs
{
class Entity;

using ComponentId = size_t;
ComponentId generate_component_id();

using ComponentType = size_t;
ComponentType generate_component_type();

class Component;
template<class T>
ComponentType get_component_type()
{
    static ComponentType type_id = generate_component_type();
    return type_id;
}

};

#endif
