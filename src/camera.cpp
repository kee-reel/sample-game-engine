#include "camera.h"

#include "includes.h"
#include "util.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(int width, int height) :
	m_mouse_inited(false)
{
	update_aspect(width, height);
	recalc();
}

const glm::vec3 &Camera::get_pos()
{
	return m_transform.get_pos();
}

const glm::mat4 &Camera::get_view()
{
	return m_view;
}

void Camera::mouse(double xpos, double ypos)
{
	if (!m_mouse_inited)
	{
		m_last_x = xpos;
		m_last_y = ypos;
		m_mouse_inited = true;
	}
  
	float xoffset = xpos - m_last_x;
	float yoffset = m_last_y - ypos; 
	m_last_x = xpos;
	m_last_y = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

    auto &&rot = m_transform.get_rot();
	if(rot.x + xoffset > 89.0f || rot.x + xoffset < -89.0f)
        xoffset = 0;
    m_transform.rotate({yoffset, xoffset, 0});
	recalc();
}

void Camera::move(glm::vec3 mov)
{
    m_transform.move(mov);
	recalc();
}

void Camera::update_aspect(int width, int height)
{
	m_projection = glm::perspective(glm::radians(45.), (double)width/height, 0.1, 1000.);
}

void Camera::use(std::shared_ptr<Shader> shader)
{
	shader->set_mat4("projection", m_projection);
}

void Camera::recalc()
{
    auto &&pos = m_transform.get_pos();
	m_view = glm::lookAt(pos, pos + m_transform.front(), m_transform.up());
}
