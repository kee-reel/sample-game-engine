#ifndef SHADER_CONFIG_H_
#define SHADER_CONFIG_H_
#include "includes.h"
#include "texture.h"
#include "shader.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace sge
{

class ShaderConfig
{
	struct TextureWrapper
	{
		int num;
		std::string name;
		std::shared_ptr<Texture> texture;
	};
	enum Field
	{
		TEXTURE,
		VEC3,
		FLOAT,
        BOOL,
        STRING,
	};
	std::map<std::string, Field> s_name_to_field = {
		{"texture", TEXTURE},
		{"vec3", VEC3},
		{"float", FLOAT},
		{"bool", BOOL},
		{"string", STRING},
	};

public:
    void load(const std::string &path);
    void load(json &&config);
	void apply(const std::shared_ptr<Shader> &shader, const std::string &prefix="");

protected:
    virtual void parse_field(const std::string &type_str, const std::string &name, nlohmann::basic_json<>& value);
    virtual void create_components();

private:
	std::string m_prefix;

	std::map<std::string, std::string> m_temp_textures;
	std::map<std::string, glm::vec3> m_temp_vectors;
	std::map<std::string, float> m_temp_floats;

	std::vector<TextureWrapper> m_textures;
	std::map<std::string, glm::vec3> m_vectors;
	std::map<std::string, float> m_floats;
};

};
#endif
