#include <list>

#include "shader_config.h"

#include "resource_loader.h"
#include "util.h"

ShaderConfig::ShaderConfig(std::string path) :
	m_is_ok(false),
	m_path(path)
{
}

void ShaderConfig::apply(const std::shared_ptr<Shader> &shader)
{
	if(!is_ok())
		return;
	shader->use();

	for(const auto &t : m_textures)
		t.texture->use(t.num, t.name, shader);
	for(const auto &f : m_floats)
		shader->set_float(f.first, f.second);
	for(const auto &vec : m_vectors)
		shader->set_vec3(vec.first, vec.second);
}

void ShaderConfig::reload(bool force)
{
	std::string data = read_file(m_path);
	if(data.empty())
		return;
	std::istringstream stream(data);
	std::string line;    
	std::string type_delim = ":";
	std::string name_delim = "=";
	int i = 0;
	std::list<std::string> errors;
	while (std::getline(stream, line)) {
		if(line.size() == 0)
			continue;
		++i;
		int type_pos = line.find(type_delim);
		int name_pos = line.find(name_delim);
		if(type_pos < 0 || name_pos < 0)
		{
			std::ostringstream ss;
			ss << "No delimeters \" \" or \"=\" on the line: " << i;
			errors.push_back(ss.str());
			continue;
		}
		auto type_str = line.substr(0, type_pos);
		auto name = line.substr(type_pos+1, name_pos-type_pos-1);
		auto value = line.substr(name_pos+1);
        auto err = parse_field(type_str, name, value);
        if(!err.empty())
            errors.push_back(err);
	}
	if(errors.size() > 0)
	{
		std::cerr << "In file " << m_path << ":" << std::endl;
		for(const auto &str : errors)
			std::cerr << str << std::endl;
		return;
	}

    create_components(force);
	m_is_ok = true;
}

std::string ShaderConfig::parse_field(const std::string &type_str, const std::string &name, const std::string &value)
{
    auto iter = s_name_to_field.find(type_str);
    if(iter == s_name_to_field.end())
    {
        std::ostringstream ss;
        ss << "Unknown field type \"" << type_str;
        return ss.str();
    }

    switch(iter->second)
    {
        case Field::TEXTURE:
            m_temp_textures[name] = value;
            break;
        case Field::VEC3:
        {
            std::istringstream iss(value);
            glm::vec3 v;
            iss >> v.x >> v.y >> v.z;
            m_temp_vectors[name] = v;
        }
        break;
        case Field::FLOAT:
        {
            std::istringstream iss(value);
            float v;
            iss >> v;
            m_temp_floats[name] = v;
        }
    }
    return "";
}

void ShaderConfig::create_components(bool force)
{
	int tex_i = 0;
	m_textures.clear();
	for(auto iter = m_temp_textures.begin(); iter != m_temp_textures.end(); ++iter)
	{
		auto texture = ResourceLoader::instance().get_texture(iter->second);
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
