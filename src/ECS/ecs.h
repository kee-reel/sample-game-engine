#ifndef ECS_ECS_
#define ECS_ECS_

#include <set>

#include "../includes.h"
#include "entity.h"

class Camera;
class Light;

namespace ecs
{

class ECS
{
public:
    ECS();

    std::shared_ptr<Entity> add_entity();
    bool remove_entity(std::weak_ptr<Entity> entity);

    template<class T, class... TArgs>
    void add_component(std::weak_ptr<Entity> entity, TArgs&&... args)
    {
        assert(!entity.expired());
        auto entity_lock = entity.lock();
        assert(!entity_lock->has_component<T>());

        auto component = std::make_unique<T>(std::move(entity), std::forward<TArgs>(args)...);
        auto id = component->id();
        auto type = get_component_type<T>();

        std::lock_guard<std::mutex> lock(m_mutex);
        entity_lock->add_component<T>(std::move(component));
        auto& components = m_components[type];
        components.insert(entity_lock->get_component<T>());
    }

    void update(const std::shared_ptr<Camera> &camera, const std::list<std::shared_ptr<Light>> &lights);

private:
    std::mutex m_mutex;
    std::set<std::shared_ptr<Entity>> m_entities;
    std::map<ComponentType, std::set<Component*>> m_components;
};

};

#endif
