#ifndef ECS_ECS_
#define ECS_ECS_

#include <set>
#include <queue>

#include "../includes.h"
#include "entity.h"
#include "system.h"

namespace ecs
{

class ECS
{
public:
    ECS() = default;

    template<class ComponentT, class SystemT>
    void add_system()
    {
        auto type = get_component_type<ComponentT>();
        assert(m_systems.find(type) == m_systems.end());
        auto& components = get_components(type);
        m_systems[type] = std::make_unique<SystemT>();
    }

    EntityId add_entity();

    template<class T, class Args...>
    void add_component(EntityId id, Args &&args...)
    {
        Entity &ent = m_entities[id];
        ComponentId id = generate_component_id();
        ComponentType type = generate_component_type();
        auto &components = m_components[type];
        components[id] = std::make_unique<T>(std::forward<Args>(args)...);
        ent[type] = id;
    }

    void update();

private:
    std::map<EntityId, Entity> m_entities;
    std::map<ComponentType, std::map<ComponentId, std::unique_ptr<Component>>> m_components;
    std::vector<std::unique_ptr<System>> m_systems;
};

};

#endif
