#include "camera.h"

#include "includes.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(int width, int height) :
	m_front(glm::vec3(0., 0., -1.)),
	m_up(glm::vec3(0., 1., 0.)),
	m_mouse_inited(false)
{
    m_transform.set_pos(0, 0, 0);
    m_transform.set_rot(-90, 0, 0);
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

void Camera::move(glm::vec3 move)
{
    glm::vec3 pos = m_transform.get_pos();
	pos += move.x * glm::normalize(glm::cross(m_front, m_up));
	pos += move.z * m_front;
    m_transform.set_pos(pos);
	recalc();
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
	float yaw = rot.x + xoffset;
	float pitch = rot.y + yoffset;

	if(pitch > 89.0f)
		pitch = 89.0f;
	if(pitch < -89.0f)
		pitch = -89.0f;

    m_transform.set_rot(rot.x + xoffset, rot.y + yoffset, rot.z);

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	m_front = glm::normalize(direction);
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
	m_view = glm::lookAt(pos, pos + m_front, m_up);
}
