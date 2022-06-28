#include <algorithm>

#include "resource_loader.h"

namespace res
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

std::shared_ptr<Shader> Loader::get_shader(const std::vector<std::string> &paths_, bool reload)
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
	if(iter != m_shaders.end() && !reload)
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

std::shared_ptr<Mesh> Loader::get_mesh()
{
	if(m_meshes.size())
	{
		return m_meshes.at(0);
	}
	std::shared_ptr<Mesh> mesh(new Mesh());
	m_meshes.push_back(mesh);
	return mesh;
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

std::shared_ptr<Component> Loader::get_by_uid(unsigned long int uid)
{
	return m_components.find(uid)->second;
}

void Loader::set_base_path(const std::string &path)
{
    m_base_path = path;
}

void Loader::add_component(const std::shared_ptr<Component> component)
{
	m_components[component->uid] = component;
}
};
