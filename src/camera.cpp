#include "camera.h"

#include "includes.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(int width, int height) :
	m_pos(glm::vec3(0., 0., 0.)),
	m_front(glm::vec3(0., 0., -1.)),
	m_up(glm::vec3(0., 1., 0.)),
	m_mouse_inited(false),
	m_yaw(-90.),
	m_pitch(0.)
{
	update_aspect(width, height);
	recalc();
}

const glm::vec3 &Camera::get_pos()
{
	return m_pos;
}

const glm::mat4 &Camera::get_view()
{
	return m_view;
}

void Camera::move(glm::vec3 move)
{
	m_pos += move.x * glm::normalize(glm::cross(m_front, m_up));
	m_pos += move.z * m_front;
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

	m_yaw += xoffset;
	m_pitch += yoffset;

	if(m_pitch > 89.0f)
		m_pitch = 89.0f;
	if(m_pitch < -89.0f)
		m_pitch = -89.0f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	direction.y = sin(glm::radians(m_pitch));
	direction.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
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
	m_view = glm::lookAt(m_pos, m_pos + m_front, m_up);
}
