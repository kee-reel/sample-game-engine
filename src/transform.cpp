#include "transform.h"

#include "includes.h"
#include <glm/gtc/matrix_transform.hpp>

const glm::vec3 UP = {0.0f, 1.0f, 0.0f};

Transform::Transform(const glm::vec3 &pos, const glm::vec3 &rot, const glm::vec3 &scale) :
	m_pos(pos),
	m_rot(rot),
	m_scale(scale)
{
	recalc();
}

Transform::~Transform()
{

}

Transform& Transform::operator=(Transform &&other)
{
    std::scoped_lock guard(m_mutex, other.m_mutex);
    std::swap(m_pos, other.m_pos);
    std::swap(m_rot, other.m_rot);
    std::swap(m_scale, other.m_scale);
    std::swap(m_model, other.m_model);
    std::swap(m_front, other.m_front);
    return *this;
}

glm::mat4 Transform::get_model() const
{
    std::lock_guard<std::mutex> guard(m_mutex);
	return m_model;
}

glm::vec3 Transform::get_pos() const
{
    std::lock_guard<std::mutex> guard(m_mutex);
    return m_pos;
}

glm::vec3 Transform::get_rot() const
{
    std::lock_guard<std::mutex> guard(m_mutex);
    return m_rot;
}

glm::vec3 Transform::get_scale() const
{
    std::lock_guard<std::mutex> guard(m_mutex);
    return m_scale;
}

glm::vec3 Transform::front() const
{
    std::lock_guard<std::mutex> guard(m_mutex);
    return m_front;
}

glm::vec3 Transform::up() const
{
    return UP;
}

void Transform::set_pos(glm::vec3 pos)
{
    std::lock_guard<std::mutex> guard(m_mutex);
	m_pos = pos;
	recalc();
}

void Transform::set_rot(glm::vec3 rot)
{
    std::lock_guard<std::mutex> guard(m_mutex);
	m_rot = rot;
	recalc();
}

void Transform::set_scale(glm::vec3 scale)
{
    std::lock_guard<std::mutex> guard(m_mutex);
	m_scale = scale;
	recalc();
}

void Transform::move(glm::vec3 diff)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    auto right = glm::normalize(glm::cross(m_front, UP));
	m_pos += right * diff.x;
	m_pos += glm::normalize(glm::cross(m_front, right)) * diff.y;
	m_pos += diff.z * m_front;
	recalc();
}

void Transform::rotate(glm::vec3 diff)
{
    std::lock_guard<std::mutex> guard(m_mutex);
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
