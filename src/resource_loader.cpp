#include <algorithm>

#include "resource_loader.h"

namespace sge
{
Loader g_res_loader;

Loader &Loader::instance()
{
	return g_res_loader;
}

std::shared_ptr<Material> Loader::get_material(const std::string &path)
{
	auto iter = m_materials.find(path);
	if(iter != m_materials.end())
	{
		return iter->second;
	}
	std::shared_ptr<Material> material(new Material(m_base_path + path));
	m_materials[path] = material;
	return material;
}

std::shared_ptr<Shader> Loader::get_shader(const std::vector<std::string> &paths_)
{
	size_t key;
	std::hash<std::string> hasher;
    std::vector<std::string> paths(paths_.size());
	for(int i = 0; i < paths_.size(); i++)
	{
		key += hasher(paths_[i]);
        paths[i] = m_base_path + paths_[i];
	}

	auto iter = m_shaders.find(key);
	if(iter != m_shaders.end())
	{
		return iter->second;
	}
	std::shared_ptr<Shader> shader(new Shader(paths));
	m_shaders[key] = shader;
	return shader;
}

std::shared_ptr<Texture> Loader::get_texture(const std::string &path)
{
	auto iter = m_textures.find(path);
	if(iter != m_textures.end())
	{
		return iter->second;
	}
	std::shared_ptr<Texture> texture(new Texture(m_base_path + path));
	m_textures[path] = texture;
	return texture;
}

std::shared_ptr<Light> Loader::get_light(std::shared_ptr<GameObject> game_object, const std::string &path)
{
	auto iter = m_lights.find(path);
	if(iter != m_lights.end())
	{
		return iter->second;
	}
	std::shared_ptr<Light> light(new Light(m_base_path + path, game_object));
	m_lights[path] = light;
	return light;
}

std::shared_ptr<Model> Loader::get_model(const std::string &path, bool flip_uv)
{
	auto iter = m_models.find(path);
	if(iter != m_models.end())
	{
		return iter->second;
	}
	std::shared_ptr<Model> model = std::make_shared<Model>(m_base_path + path, flip_uv);
	m_models[path] = model;
	return model;
}

std::shared_ptr<Script> Loader::get_script(const std::string &path)
{
	auto iter = m_scripts.find(path);
	if(iter != m_scripts.end())
	{
		return iter->second;
	}
	return m_scripts[path] = std::make_shared<Script>(m_base_path + path);
}

void Loader::set_base_path(const std::string &path)
{
    m_base_path = path;
}
};
