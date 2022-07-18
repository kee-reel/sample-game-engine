#ifndef MATERIAL_H_
#define MATERIAL_H_
#include "includes.h"
#include "texture.h"
#include "shader.h"
#include "camera.h"
#include "light.h"
#include "shader_config.h"

namespace sge
{

class Material : public ShaderConfig
{
public:
	Material(std::string path);
	Material(nlohmann::json &&data);
    virtual ~Material() {}

	void use(const std::shared_ptr<Camera> &camera, const std::list<std::shared_ptr<Light>> &lights);
	void use_model(glm::mat4 &&mat);

private:
    void parse_field(const std::string &type_str, const std::string &name, nlohmann::basic_json<>& value) override;
    void create_components() override;

private:
    std::shared_ptr<Shader> m_shader;
	std::vector<std::string> m_temp_shaders;
    bool m_is_emitting;
};

};
#endif
