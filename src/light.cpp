#include "light.h"

#include "util.h"

Light::Light(const std::string &path, std::shared_ptr<GameObject> game_object) :
	Component(Component::LIGHT),
    ShaderConfig(path),
    m_game_object(game_object)
{
    reload();
}

void Light::apply(const std::shared_ptr<Shader> &shader)
{
    ShaderConfig::apply(shader);
    shader->set_vec3("light.position", m_game_object->transform.get_pos());
}
