#ifndef TRANSFORM_H_
#define TRANSFORM_H_
#include <mutex>

#include "shader.h"

class Transform
{
public:
	Transform(
            const glm::vec3 &pos = glm::vec3{0., 0., 0.},
            const glm::vec3 &rot = glm::vec3{0., 0., 0.},
            const glm::vec3 &scale = glm::vec3{1., 1., 1.});
	~Transform();
    Transform& operator=(Transform &&other);
	glm::mat4 get_model() const;

	glm::vec3 get_pos() const;
	glm::vec3 get_rot() const;
	glm::vec3 get_scale() const;

	glm::vec3 front() const;
	glm::vec3 up() const;

	void set_pos(glm::vec3 pos);
	void set_rot(glm::vec3 rot);
	void set_scale(glm::vec3 scale);

	void move(glm::vec3 diff);
	void rotate(glm::vec3 diff);

private:
	void recalc();

private:
    mutable std::mutex m_mutex;
	glm::mat4 m_model;

	glm::vec3 m_pos;
	glm::vec3 m_rot;
	glm::vec3 m_scale;

	glm::vec3 m_front;
};
#endif
