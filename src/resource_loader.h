#ifndef RESOURCE_LOADER_H_
#define RESOURCE_LOADER_H_
#include "includes.h"

#include "material.h"
#include "shader.h"
#include "texture.h"
#include "mesh.h"

class ResourceLoader
{
public:
	static ResourceLoader &instance();
	std::shared_ptr<Material> get_material(const std::string &path);	
	std::shared_ptr<Shader> get_shader(const std::vector<std::string> &path, bool reload);
	std::shared_ptr<Texture> get_texture(const std::string &path);	
	std::shared_ptr<Mesh> get_mesh();
	std::shared_ptr<Component> get_by_uid(unsigned long int uid);

private:
	void add_component(const std::shared_ptr<Component> component);

private:
	std::map<std::string, std::shared_ptr<Material>> m_materials;
	std::map<size_t, std::shared_ptr<Shader>> m_shaders;
	std::map<std::string, std::shared_ptr<Texture>> m_textures;
	std::vector<std::shared_ptr<Mesh>> m_meshes;
	std::map<unsigned long int, std::shared_ptr<Component>> m_components;
};

#endif
