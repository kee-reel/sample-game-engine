#ifndef ECS_ENTITY_
#define ECS_ENTITY_

#include <array>

#include "../includes.h"
#include "component.h"

namespace ecs
{
class ECS;

using EntityId = size_t;
EntityId generate_entity_id();

const size_t components_count = 8;

class Entity
{
    friend ECS;
public:
    Entity();
    EntityId id();

    template<class T>
    Component* get_component()
    {
        return m_components[get_component_type<T>()].get();
    }

    Component* get_component(ComponentType type)
    {
        return m_components[type].get();
    }

    template<class T>
    bool has_component()
    {
        return m_components[get_component_type<T>()] != nullptr;
    }

    bool has_component(ComponentType type)
    {
        return m_components[type] != nullptr;
    }

private:
    template<class T>
    void add_component(std::unique_ptr<Component> &&component)
    {
        m_components[get_component_type<T>()] = std::move(component);
    }

    template<class T>
    void remove_component()
    {
        m_components[get_component_type<T>()] = nullptr;
    }

private:
    const EntityId m_id;
    std::array<std::unique_ptr<Component>, components_count> m_components;
};

};
#endif
