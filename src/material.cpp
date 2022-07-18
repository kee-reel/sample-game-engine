#include <list>
#include "material.h"

#include "resource_loader.h"
#include "util.h"

namespace sge
{

Material::Material(std::string path) :
    m_is_emitting(false)
{
    load(path);
}

Material::Material(nlohmann::json &&data) :
    m_is_emitting(false)
{
    load(std::move(data));
}

void Material::use(const std::shared_ptr<Camera> &camera, const std::list<std::shared_ptr<Light>> &lights)
{
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
}

void Material::use_model(glm::mat4 &&model)
{
	m_shader->set_mat4("model", std::move(model));
}

void Material::parse_field(const std::string &type_str, const std::string &name, nlohmann::basic_json<>& value)
{
    if(!type_str.compare("shader"))
        m_temp_shaders.push_back(value);
    else if(!name.compare("is_emitting"))
        m_is_emitting = bool(value);
    else
        return ShaderConfig::parse_field(type_str, name, value);
}

void Material::create_components()
{
    m_shader = Loader::instance().get_shader(m_temp_shaders);
    m_temp_shaders.clear();

    ShaderConfig::create_components();
}

};
