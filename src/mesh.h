#ifndef MESH_H_
#define MESH_H_
#include "shader.h"
#include "texture.h"
#include "transform.h"
#include "camera.h"
#include "component.h"

class Mesh : public Component
{
	struct Vertex {
		glm::vec3 pos;
		glm::vec3 norm;
		glm::vec2 tex;
	};
	struct Index {
		GLuint indices[3];
	};
public:
	virtual ~Mesh();

public:
	Mesh();
	void draw(int instances_count = 1);
	void reload();

private:
	std::vector<Vertex> m_vertices;
	std::vector<Index> m_indices;
	GLuint m_VAO;
	GLuint m_EBO;
	GLuint m_VBO;
};
#endif
