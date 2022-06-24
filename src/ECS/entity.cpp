#include "entity.h"

namespace ecs
{
EntityId generate_entity_id()
{
    static EntityId id = 0;
    return id++;
}

Entity::Entity() :
    m_id(generate_entity_id())
{ }

EntityId Entity::id()
{
    return m_id;
}
};
