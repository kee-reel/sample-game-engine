#ifndef TRANSFORM_H_
#define TRANSFORM_H_
#include "shader.h"

#include "sge.h"

class Transform : public sge::ITransform
{
public:
	void set_pos(float x, float y, float z) override;
	void set_rot(float x, float y, float z) override;
	void set_scale(float x, float y, float z) override;

public:
	Transform();
	~Transform();
	const glm::mat4 &get_model();
	const glm::vec3& get_pos();
	const glm::vec3& get_rot();
	const glm::vec3& get_scale();
	void set_pos(glm::vec3 pos);
	void set_rot(glm::vec3 rot);
	void set_scale(glm::vec3 scale);

private:
	void recalc();

private:
	glm::mat4 m_model;

	glm::vec3 m_pos;
	glm::vec3 m_rot;
	glm::vec3 m_scale;
};
#endif
