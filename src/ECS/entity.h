#ifndef ECS_ENTITY_
#define ECS_ENTITY_

#include <array>

#include "../includes.h"
#include "component.h"

namespace ecs
{
using EntityId = size_t;
EntityId generate_entity_id();

const size_t MAX_COMPONENTS = 4;
using Entity = std::array<ComponentId, MAX_COMPONENTS>;
};
#endif
