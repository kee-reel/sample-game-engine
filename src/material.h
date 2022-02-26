#ifndef MATERIAL_H_
#define MATERIAL_H_
#include "includes.h"
#include "texture.h"
#include "shader.h"
#include "camera.h"
#include "component.h"

class Material : public Component
{
	struct TextureWrapper
	{
		int num;
		std::string name;
		std::shared_ptr<Texture> texture;
	};
	enum Field
	{
		SHADER,
		TEXTURE,
		VEC3,
		FLOAT,
	};
	std::map<std::string, Field> s_name_to_field = {
		{"shader", SHADER},
		{"texture", TEXTURE},
		{"vec3", VEC3},
		{"float", FLOAT},
	};

public:
	Material(std::string path);

	void use(const std::shared_ptr<Camera> &camera);
	void use_model(const glm::mat4 &mat);
	void reload(bool reload=true);

private:
	bool m_is_ok;
	std::string m_path;
	std::shared_ptr<Shader> m_shader;
	std::vector<TextureWrapper> m_textures;
	std::map<std::string, glm::vec3> m_vectors;
	std::map<std::string, float> m_floats;
};
#endif
