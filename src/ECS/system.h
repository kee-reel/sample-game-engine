#ifndef ECS_SYSTEM_
#define ECS_SYSTEM_

#include <set>
#include <memory>

#include "component.h"

namespace ecs
{

class System
{
public:
    System(std::shared_ptr<std::set<Component*>> components) :
        m_components{components}
    {}
    virtual ~System() {}
    virtual void on_added(Component* component) {}
    virtual void on_removed(Component* component) {}
    virtual void update() {}

protected:
    std::shared_ptr<std::set<Component*>> m_components;
};

};
#endif
