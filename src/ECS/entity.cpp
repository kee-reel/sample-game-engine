#include "entity.h"

namespace ecs
{
EntityId generate_entity_id()
{
    static EntityId id = 1;
    return id++;
}
};
