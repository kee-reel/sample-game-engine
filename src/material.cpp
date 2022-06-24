#include <list>
#include "material.h"

#include "resource_loader.h"
#include "util.h"

Material::Material(std::string path) :
	Component(Component::MATERIAL),
    ShaderConfig(path),
    m_is_emitting(false)
{
    reload();
}

void Material::use(const std::shared_ptr<Camera> &camera, const std::list<std::shared_ptr<Light>> &lights)
{
	if(!is_ok())
		return;
	
    apply(m_shader);
	camera->use(m_shader);
    m_shader->set_uint("lights_count", lights.size());
    if(!m_is_emitting)
    {
        int i = 0;
        for(const auto& light : lights)
        {
            std::ostringstream ss;
            ss << "lights[" << i++ << "].";
            light->apply(m_shader, ss.str());
        }
    }
	m_shader->set_vec3("viewPosition", camera->get_pos());
	m_shader->set_mat4("view", camera->get_view());
}

void Material::use_model(glm::mat4 &&model)
{
	if(!is_ok())
		return;
	m_shader->set_mat4("model", std::move(model));
}

std::string Material::parse_field(const std::string &type_str, const std::string &name, nlohmann::basic_json<>& value)
{
    if(!type_str.compare("shader"))
    {
        m_temp_shaders.push_back(value);
        return "";
    }
    else if(!name.compare("is_emitting"))
    {
        m_is_emitting = bool(value);
    }
    return ShaderConfig::parse_field(type_str, name, value);
}

void Material::create_components(bool force)
{
    m_shader = ResourceLoader::instance().get_shader(m_temp_shaders, force);
    m_temp_shaders.clear();

    ShaderConfig::create_components(force);
}
