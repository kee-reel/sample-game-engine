#ifndef LIGHT_H_
#define LIGHT_H_
#include "includes.h"
#include "texture.h"
#include "shader.h"
#include "camera.h"
#include "component.h"

class Light : public Component
{
	enum Type
	{
        DIRECTIONAL,
        POINT,
        SPOT,
	};

public:
	Light(Type type, glm::vec3 color);

	void use(const std::shared_ptr<Camera> &camera);
	void use_model(const glm::mat4 &mat);
	void reload(bool reload=true);

private:
    float m_radius;
    glm::vec3 m_color;
};
#endif
