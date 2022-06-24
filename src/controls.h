#ifndef H_CONTROLS_
#define H_CONTROLS_
#include "includes.h"

#include "transform.h"
#include "component.h"

class Controls : public Component
{
public:
	Camera(std::weak_ptr<Entity> &&entity);
    void mouse(double xpos, double ypos);
    void move(glm::vec3 mov);

private:
	bool m_mouse_inited;
	double m_last_x, m_last_y;
};
#endif
