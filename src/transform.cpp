#include "transform.h"

#include "includes.h"
#include <glm/gtc/matrix_transform.hpp>

Transform::Transform() :
	m_pos(glm::vec3(0., 0., 0.)),
	m_rot(glm::vec3(-90., 0., 0.)),
	m_scale(glm::vec3(1., 1., 1.))
{
	recalc();
}

Transform::~Transform()
{

}

const glm::mat4 &Transform::get_model()
{
	return m_model;
}

const glm::vec3& Transform::get_pos()
{
    return m_pos;
}

const glm::vec3& Transform::get_rot()
{
    return m_rot;
}

const glm::vec3& Transform::get_scale()
{
    return m_scale;
}

const glm::vec3& Transform::front()
{
    return m_front;
}

const glm::vec3& Transform::up()
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
