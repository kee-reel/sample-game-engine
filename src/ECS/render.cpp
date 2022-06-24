#include "render.h"

#include "../includes.h"
#include <glm/gtc/matrix_transform.hpp>

namespace ecs
{
Render::Render(std::weak_ptr<Entity> &&entity, std::shared_ptr<Material> material, std::shared_ptr<Mesh> mesh) :
    Component(std::move(entity)),
    m_material(std::move(material)),
    m_mesh(std::move(mesh))
{

}

Render::~Render()
{

}

std::shared_ptr<Material> Render::get_material()
{
    return m_material;
}

std::shared_ptr<Mesh> Render::get_mesh()
{
    return m_mesh;
}
};
