#include <list>

#include "material.h"

#include "resource_loader.h"
#include "util.h"

Material::Material(std::string path) :
	Component(Component::MATERIAL),
	m_is_ok(false),
	m_path(path)
{
	reload();
}

void Material::use(const std::shared_ptr<Camera> &camera)
{
	if(!m_is_ok)
		return;
	m_shader->use();

	camera->use(m_shader);

	m_shader->set_vec3("viewPosition", camera->get_pos());
	m_shader->set_mat4("view", camera->get_view());

	for(const auto &t : m_textures)
		t.texture->use(t.num, t.name, m_shader);
	for(const auto &f : m_floats)
		m_shader->set_float(f.first, f.second);
	for(const auto &vec : m_vectors)
		m_shader->set_vec3(vec.first, vec.second);
}

void Material::use_model(const glm::mat4 &model)
{
	if(!m_is_ok)
		return;
	m_shader->set_mat4("model", model);
}

void Material::reload(bool force)
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
	std::vector<std::string> shaders;
	std::map<std::string, std::string> textures;
	std::map<std::string, glm::vec3> vectors;
	std::map<std::string, float> floats;
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
		auto iter = s_name_to_field.find(type_str);
		if(iter == s_name_to_field.end())
		{
			std::ostringstream ss;
			ss << "Unknown field type \"" << type_str << "\" on line " << i;
			errors.push_back(ss.str());
			continue;
		}

		auto name = line.substr(type_pos+1, name_pos-type_pos-1);
		auto value = line.substr(name_pos+1);
		switch(iter->second)
		{
			case Field::SHADER:
				shaders.push_back(value);
				break;
			case Field::TEXTURE:
				textures[name] = value;
				break;
			case Field::VEC3:
				{
				std::istringstream iss(value);
				glm::vec3 v;
				iss >> v.x >> v.y >> v.z;
				vectors[name] = v;
				}
				break;
			case Field::FLOAT:
				{
				std::istringstream iss(value);
				float v;
				iss >> v;
				floats[name] = v;
				}
				break;
		}
	}
	if(errors.size() > 0)
	{
		std::cerr << "In file " << m_path << ":" << std::endl;
		for(const auto &str : errors)
			std::cerr << str << std::endl;
		return;
	}
	m_shader = ResourceLoader::instance().get_shader(shaders, force);
	int tex_i = 0;
	m_textures.clear();
	for(auto iter = textures.begin(); iter != textures.end(); ++iter)
	{
		auto texture = ResourceLoader::instance().get_texture(iter->second);
		m_textures.push_back(TextureWrapper({tex_i++, iter->first, texture}));
	}
	m_vectors.clear();
	for(auto iter = vectors.begin(); iter != vectors.end(); ++iter)
	{
		m_vectors[iter->first] = iter->second;
	}
	m_floats.clear();
	for(auto iter = floats.begin(); iter != floats.end(); ++iter)
	{
		m_floats[iter->first] = iter->second;
	}
	m_is_ok = true;
}
