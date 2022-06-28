#include <list>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "shader_config.h"

#include "resource_loader.h"
#include "util.h"

ShaderConfig::ShaderConfig(std::string path) :
	m_is_ok(false),
	m_path(path)
{
}

void ShaderConfig::apply(const std::shared_ptr<Shader> &shader, const std::string &prefix)
{
	if(!is_ok())
		return;
	shader->use();

	for(const auto &t : m_textures)
		t.texture->use(t.num, t.name, shader);
	for(const auto &f : m_floats)
		shader->set_float(prefix + f.first, f.second);
	for(const auto &vec : m_vectors)
		shader->set_vec3(prefix + vec.first, vec.second);
}

void ShaderConfig::reload(bool force)
{
    std::ifstream f(m_path);
    json config;
    f >> config;
    f.close();

    std::list<std::string> errors;
    for(auto iter = config.begin(); iter != config.end(); iter++)
    {
        for(auto v_iter = iter.value().begin(); v_iter != iter.value().end(); v_iter++)
        {
            auto err = parse_field(iter.key(), v_iter.key(), v_iter.value());
            if(!err.empty())
                errors.push_back(err);
        }
    }
    if(errors.size())
    {
        std::cout << "Errors during shader processing:" << std::endl;
        for(auto &e : errors)
            std::cout << e << std::endl;
        return;
    }

    create_components(force);
	m_is_ok = true;
}

std::string ShaderConfig::parse_field(const std::string &type_str, const std::string &name, nlohmann::basic_json<>& value)
{
    auto iter = s_name_to_field.find(type_str);
    if(iter == s_name_to_field.end())
    {
        std::ostringstream ss;
        ss << "Unknown field type " << type_str;
        return ss.str();
    }

    switch(iter->second)
    {
        case Field::TEXTURE:
            m_temp_textures[name] = value;
            break;
        case Field::VEC3:
            m_temp_vectors[name] = glm::vec3{value[0], value[1], value[2]};
            break;
        case Field::FLOAT:
            m_temp_floats[name] = value;
    }
    return "";
}

void ShaderConfig::create_components(bool force)
{
	int tex_i = 0;
	m_textures.clear();
	for(auto iter = m_temp_textures.begin(); iter != m_temp_textures.end(); ++iter)
	{
		auto texture = res::Loader::instance().get_texture(iter->second);
		m_textures.push_back(TextureWrapper({tex_i++, iter->first, texture}));
	}
    m_temp_textures.clear();

	m_vectors.clear();
	for(auto iter = m_temp_vectors.begin(); iter != m_temp_vectors.end(); ++iter)
	{
		m_vectors[iter->first] = iter->second;
	}
	m_temp_vectors.clear();

	m_floats.clear();
	for(auto iter = m_temp_floats.begin(); iter != m_temp_floats.end(); ++iter)
	{
		m_floats[iter->first] = iter->second;
	}
	m_temp_floats.clear();
}
