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
    static_assert(std::is_base_of<Component, T>(), "Can use only Component-based classes");
    static ComponentType type_id = generate_component_type();
    return type_id;
}

class Component
{
public:
	Component(std::weak_ptr<Entity> &&entity);
    virtual ~Component() {}
    ComponentId id();
    std::shared_ptr<Entity> get_entity();

protected:
    const ComponentId m_id;
    std::weak_ptr<Entity> m_entity;
};

};

#endif
