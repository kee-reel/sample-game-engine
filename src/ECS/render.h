#ifndef ECS_RENDER_H_
#define ECS_RENDER_H_

#include "component.h"
#include "../material.h"
#include "../mesh.h"

namespace ecs
{

class Render : public Component
{
public:
	Render(std::weak_ptr<Entity> &&entity, std::shared_ptr<Material> material, std::shared_ptr<Mesh> mesh);
	virtual ~Render();

    std::shared_ptr<Material> get_material();
    std::shared_ptr<Mesh> get_mesh();

private:
    std::shared_ptr<Mesh> m_mesh;
    std::shared_ptr<Material> m_material;
};

};
#endif
