#ifndef H_CAMERA_
#define H_CAMERA_
#include "includes.h"

#include "shader.h"
#include "transform.h"

class Camera
{
public:
	Camera(int width, int height);
	const glm::mat4 &get_view();
	const glm::vec3 &get_pos();
	void use(std::shared_ptr<Shader> shader);
	void update_aspect(int width, int height);
	Transform &transform() { return m_transform; }
    void mouse(double xpos, double ypos);
    void move(glm::vec3 mov);

private:
	void recalc();

private:
	glm::mat4 m_view;
	glm::mat4 m_projection;
	bool m_mouse_inited;
	double m_last_x, m_last_y;
    Transform m_transform;
};
#endif
