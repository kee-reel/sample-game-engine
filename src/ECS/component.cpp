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
    static ComponentId id = 1;
    return id++;
}
};
