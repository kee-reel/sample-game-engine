#ifndef TRANSFORM_H_
#define TRANSFORM_H_
#include "shader.h"

#include "sge.h"

class Transform : public sge::ITransform
{
public:
	Transform();
	~Transform();
	const glm::mat4 &get_model();

	const glm::vec3& get_pos() override;
	const glm::vec3& get_rot() override;
	const glm::vec3& get_scale() override;

	const glm::vec3& front() override;
	const glm::vec3& up() override;

	void set_pos(glm::vec3 pos) override;
	void set_rot(glm::vec3 rot) override;
	void set_scale(glm::vec3 scale) override;

	void move(glm::vec3 diff) override;
	void rotate(glm::vec3 diff) override;

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
