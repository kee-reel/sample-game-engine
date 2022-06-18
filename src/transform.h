#ifndef TRANSFORM_H_
#define TRANSFORM_H_
#include "shader.h"

class Transform
{
public:
	Transform();
	~Transform();
	const glm::mat4 &get_model();

	const glm::vec3& get_pos();
	const glm::vec3& get_rot();
	const glm::vec3& get_scale();

	const glm::vec3& front();
	const glm::vec3& up();

	void set_pos(glm::vec3 pos);
	void set_rot(glm::vec3 rot);
	void set_scale(glm::vec3 scale);

	void move(glm::vec3 diff);
	void rotate(glm::vec3 diff);

private:
	void recalc();

private:
	glm::mat4 m_model;

	glm::vec3 m_pos;
	glm::vec3 m_rot;
	glm::vec3 m_scale;

	glm::vec3 m_front;
	glm::vec3 m_up = {0.0f, 1.0f, 0.0f};
	glm::vec3 m_right = {1.0f, 0.0f, 0.0f};
};
#endif
