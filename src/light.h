#ifndef LIGHT_H_
#define LIGHT_H_
#include "includes.h"
#include "shader.h"
#include "game_object.h"
#include "shader_config.h"

namespace sge
{

class Light : public ShaderConfig
{
	enum Type
	{
        DIRECTIONAL,
        POINT,
        SPOT,
	};
	std::map<std::string, Type> s_name_to_type = {
		{"directional", DIRECTIONAL},
		{"point", POINT},
		{"spot", SPOT},
	};

public:
	Light(const std::string &path, std::shared_ptr<GameObject> game_object);
    virtual ~Light() {}
	void apply(const std::shared_ptr<Shader> &shader, const std::string &prefix);

private:
    void parse_field(const std::string &type_str, const std::string &name, nlohmann::basic_json<>& value) override;
    void create_components() override;

private:
    Type m_temp_light_type;
    Type m_light_type;
    std::shared_ptr<GameObject> m_game_object;
};

};
#endif
