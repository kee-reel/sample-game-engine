#include "controls.h"

#include "includes.h"
#include "util.h"

Controls::Controls(std::weak_ptr<Entity> &&entity) :
    Component(std::move(entity)),
	m_mouse_inited(false)
{
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

    auto entity = m_entity.lock();
    auto transform = entity->get_component<Transform>().lock();
    auto &&rot = transform->get_rot();
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
