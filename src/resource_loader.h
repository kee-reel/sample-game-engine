#ifndef RESOURCE_LOADER_H_
#define RESOURCE_LOADER_H_
#include "includes.h"

#include "material.h"
#include "shader.h"
#include "texture.h"
#include "light.h"
#include "mesh.h"
#include "script.h"

namespace res
{
class Loader
{
public:
	static Loader &instance();
	std::shared_ptr<Material> get_material(const std::string &path);	
	std::shared_ptr<Shader> get_shader(const std::vector<std::string> &path, bool reload);
	std::shared_ptr<Texture> get_texture(const std::string &path);	
	std::shared_ptr<Light> get_light(std::shared_ptr<GameObject> game_object, const std::string &path);
	std::shared_ptr<Script> get_script(const std::string &script);	
	std::shared_ptr<Mesh> get_mesh();
	std::shared_ptr<Component> get_by_uid(unsigned long int uid);
	void set_base_path(const std::string &path);

private:
	void add_component(const std::shared_ptr<Component> component);

private:
	std::map<std::string, std::shared_ptr<Material>> m_materials;
	std::map<size_t, std::shared_ptr<Shader>> m_shaders;
	std::map<std::string, std::shared_ptr<Texture>> m_textures;
	std::map<std::string, std::shared_ptr<Light>> m_lights;
	std::map<std::string, std::shared_ptr<Script>> m_scripts;
	std::vector<std::shared_ptr<Mesh>> m_meshes;
	std::map<unsigned long int, std::shared_ptr<Component>> m_components;
    std::string m_base_path;
};
};
#endif
