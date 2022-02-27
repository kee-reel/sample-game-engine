#ifndef LIGHT_H_
#define LIGHT_H_
#include "includes.h"
#include "shader.h"
#include "component.h"
#include "game_object.h"
#include "shader_config.h"

class Light : public Component, public ShaderConfig
{
	enum Type
	{
        DIRECTIONAL,
        POINT,
        SPOT,
	};

public:
	Light(const std::string &path, std::shared_ptr<GameObject> game_object);
    virtual ~Light() {}
	void apply(const std::shared_ptr<Shader> &shader) override;

private:
    std::shared_ptr<GameObject> m_game_object;
};
#endif
