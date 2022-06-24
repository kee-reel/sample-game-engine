#include "component.h"

namespace ecs
{

ComponentType generate_component_type()
{
    static ComponentType type_id = 0;
    return type_id++;
}

ComponentId generate_component_id()
{
    static ComponentId id = 0;
    return id++;
}

Component::Component(std::weak_ptr<Entity> &&entity) :
    m_id(generate_component_id()),
    m_entity(std::move(entity))
{}

ComponentId Component::id()
{
    return m_id;
}

std::shared_ptr<Entity> Component::get_entity()
{
    return m_entity.lock();
}
};
