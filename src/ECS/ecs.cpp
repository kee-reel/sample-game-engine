#include "ecs.h"

#include "render.h"
#include "transform.h"
#include "../camera.h"
#include "../light.h"

namespace ecs
{
EntityId ECS::add_entity()
{
    EntityId id = generate_entity_id();
    m_entities[id] = entity;
    return id;
}

//bool ECS::remove_entity(EntityId entity)
//{
//    std::lock_guard<std::mutex> lock(m_mutex);
//    if(entity.expired())
//        return false;
//    auto entity_lock = entity.lock();
//
//    if(m_entities.find(entity_lock) == m_entities.end())
//        return false;
//
//    for(auto iter = m_components.begin(); iter != m_components.end(); iter++)
//    {
//        auto component = entity_lock->get_component(iter->first);
//        if(component)
//            iter->second->erase(component);
//    }
//    return true;
//}
void ECS::update()
{
    for(auto iter = m_systems.begin(); iter != m_systems.end(); iter++)
        iter->second->update();
}

std::shared_ptr<std::set<Component*>>& ECS::get_components(ComponentType type)
{
        auto& components = m_components[type];
        if(!components)
            components = std::make_shared<std::set<Component*>>();
        return components;
}

};
