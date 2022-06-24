#include "transform.h"

#include "../includes.h"
#include <glm/gtc/matrix_transform.hpp>

namespace ecs
{

Transform::Transform(std::weak_ptr<Entity> &&entity) :
    Component(std::move(entity)),
	m_pos(glm::vec3(0., 0., 0.)),
	m_rot(glm::vec3(0., 0., 0.)),
	m_scale(glm::vec3(1., 1., 1.))
{
	recalc();
}

Transform::~Transform()
{

}

glm::mat4 Transform::get_model() const
{
	return m_model;
}

glm::vec3 Transform::get_pos() const
{
    return m_pos;
}

glm::vec3 Transform::get_rot() const
{
    return m_rot;
}

glm::vec3 Transform::get_scale() const
{
    return m_scale;
}

glm::vec3 Transform::front() const
{
    return m_front;
}

glm::vec3 Transform::up() const
{
    return m_up;
}

void Transform::set_pos(glm::vec3 pos)
{
	m_pos = pos;
	recalc();
}

void Transform::set_rot(glm::vec3 rot)
{
	m_rot = rot;
	recalc();
}

void Transform::set_scale(glm::vec3 scale)
{
	m_scale = scale;
	recalc();
}

void Transform::move(glm::vec3 diff)
{
    auto right = glm::normalize(glm::cross(m_front, m_up));
	m_pos += right * diff.x;
	m_pos += glm::normalize(glm::cross(m_front, right)) * diff.y;
	m_pos += diff.z * m_front;
	recalc();
}

void Transform::rotate(glm::vec3 diff)
{
    m_rot += diff;
	recalc();
}

void Transform::recalc()
{
	m_front = glm::normalize(glm::vec3{
	    cos(glm::radians(m_rot.y)) * cos(glm::radians(m_rot.x)),
	    sin(glm::radians(m_rot.x)),
	    sin(glm::radians(m_rot.y)) * cos(glm::radians(m_rot.x))
    });
	m_model = glm::mat4(1.);
	m_model = glm::translate(m_model, m_pos);
	m_model = glm::scale(m_model, m_scale);
	m_model = glm::rotate(m_model, m_rot.x, glm::vec3(1., 0., 0.));
	m_model = glm::rotate(m_model, m_rot.y, glm::vec3(0., 1., 0.));
	m_model = glm::rotate(m_model, m_rot.z, glm::vec3(0., 0., 1.));
}

};
