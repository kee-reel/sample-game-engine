#include "ecs.h"

#include "render.h"
#include "transform.h"
#include "../camera.h"
#include "../light.h"

namespace ecs
{
ECS::ECS()
{ }

std::shared_ptr<Entity> ECS::add_entity()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto entity = std::make_shared<Entity>();
    m_entities.insert(entity);
    return entity;
}

bool ECS::remove_entity(std::weak_ptr<Entity> entity)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if(entity.expired())
        return false;
    auto entity_lock = entity.lock();

    if(m_entities.find(entity_lock) == m_entities.end())
        return false;

    for(auto iter = m_components.begin(); iter != m_components.end(); iter++)
    {
        auto component = entity_lock->get_component(iter->first);
        if(component)
            iter->second.erase(component);
    }
    return true;
}
void ECS::update(const std::shared_ptr<Camera> &camera, const std::list<std::shared_ptr<Light>> &lights)
{
    auto& renders = m_components[get_component_type<Render>()];
    for(auto iter = renders.begin(); iter != renders.end(); iter++)
    {
        Render* render = static_cast<Render*>(*iter);
        Transform* transform = static_cast<Transform*>(render->get_entity()->get_component<Transform>());
        auto material = render->get_material();
		material->use(camera, lights);
        auto mesh = render->get_mesh();
        material->use_model(transform->get_model());
        mesh->draw();
	}
}

};
