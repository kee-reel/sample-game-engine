#ifndef SHADER_CONFIG_H_
#define SHADER_CONFIG_H_
#include "includes.h"
#include "texture.h"
#include "shader.h"

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
	};
	std::map<std::string, Field> s_name_to_field = {
		{"texture", TEXTURE},
		{"vec3", VEC3},
		{"float", FLOAT},
		{"bool", BOOL},
	};

public:
	ShaderConfig(std::string path);
    virtual ~ShaderConfig() {}

	virtual void apply(const std::shared_ptr<Shader> &shader);
	void reload(bool force=true);

protected:
    virtual std::string parse_field(const std::string &type_str, const std::string &name, const std::string &value);
    virtual void create_components(bool force);
    bool is_ok()
    {
        return m_is_ok;
    }

private:
	bool m_is_ok;
	std::string m_path;

	std::map<std::string, std::string> m_temp_textures;
	std::map<std::string, glm::vec3> m_temp_vectors;
	std::map<std::string, float> m_temp_floats;

	std::vector<TextureWrapper> m_textures;
	std::map<std::string, glm::vec3> m_vectors;
	std::map<std::string, float> m_floats;
};
#endif
