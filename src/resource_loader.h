#ifndef RESOURCE_LOADER_H_
#define RESOURCE_LOADER_H_
#include "includes.h"

#include "material.h"
#include "shader.h"
#include "texture.h"
#include "light.h"
#include "model.h"
#include "script.h"

namespace sge
{
class Loader
{
public:
	static Loader &instance();
	std::shared_ptr<Material> get_material(const std::string &path);	
	std::shared_ptr<Shader> get_shader(const std::vector<std::string> &path);
	std::shared_ptr<Texture> get_texture(const std::string &path);	
	std::shared_ptr<Light> get_light(std::shared_ptr<GameObject> game_object, const std::string &path);
	std::shared_ptr<Script> get_script(const std::string &script);	
	std::shared_ptr<Model> get_model(const std::string &path, bool flip_uv);
	void set_base_path(const std::string &path);

private:
	std::map<std::string, std::shared_ptr<Material>> m_materials;
	std::map<size_t, std::shared_ptr<Shader>> m_shaders;
	std::map<std::string, std::shared_ptr<Texture>> m_textures;
	std::map<std::string, std::shared_ptr<Light>> m_lights;
	std::map<std::string, std::shared_ptr<Script>> m_scripts;
	std::map<std::string, std::shared_ptr<Model>> m_models;
    std::string m_base_path;
};
};
#endif
