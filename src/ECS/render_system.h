#ifndef ECS_RENDER_SYSTEM_
#define ECS_RENDER_SYSTEM_

#include "system.h"
#include "render.h"

namespace ecs
{

class RenderSystem : public System
{
public:
    RenderSystem(std::shared_ptr<std::set<Component*>> components) :
        System{components}
    {}
    virtual ~RenderSystem() {}
    void on_added(Component* component) override {}
    void on_removed(Component* component) override {}
    void update() override
    {
        for(auto iter = m_components->begin(); iter != m_components->end(); iter++)
        {
            Render* render = static_cast<Render*>(*iter);
            Transform* transform = static_cast<Transform*>(render->get_entity()->get_component<Transform>());
            auto material = render->get_material();
            //material->use(camera, lights);
            auto mesh = render->get_mesh();
            material->use_model(transform->get_model());
            mesh->draw();
        }
    }
};

};
#endif
