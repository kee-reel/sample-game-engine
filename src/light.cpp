#include "light.h"

#include "util.h"

namespace sge
{

Light::Light(const std::string &path, std::shared_ptr<GameObject> game_object) :
    m_game_object(game_object)
{
    load(path);
}

void Light::apply(const std::shared_ptr<Shader> &shader, const std::string &prefix)
{
    ShaderConfig::apply(shader, prefix);
    switch(m_light_type)
    {
        case Type::DIRECTIONAL:
            shader->set_vec4(prefix + "position", glm::vec4(m_game_object->transform.get_pos(), 0.0f));
            break;
        case Type::POINT:
            shader->set_vec4(prefix + "position", glm::vec4(m_game_object->transform.get_pos(), 1.0f));
            break;
        case Type::SPOT:
            shader->set_vec4(prefix + "position", glm::vec4(m_game_object->transform.get_pos(), 1.0f));
            break;
    }
}

void Light::parse_field(const std::string &type_str, const std::string &name, nlohmann::basic_json<>& value)
{
    if(!name.compare("light_type"))
    {
        auto iter = s_name_to_type.find(value);
        if(iter == s_name_to_type.end())
        {
            std::ostringstream ss;
            ss << "Unknown light type \"" << value;
            throw std::runtime_error(ss.str());
        }
        m_temp_light_type = iter->second;
    }
    else
        ShaderConfig::parse_field(type_str, name, value);
}

void Light::create_components()
{
    m_light_type = m_temp_light_type;
    ShaderConfig::create_components();
}

};
