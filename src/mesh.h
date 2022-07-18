#ifndef MESH_H_
#define MESH_H_
#include "shader.h"
#include "texture.h"
#include "transform.h"
#include "camera.h"

namespace sge
{

class Mesh
{
public:
	struct Vertex {
		glm::vec3 pos;
		glm::vec3 norm;
		glm::vec2 tex;
	};
    struct Index {
        GLuint indices[3];
    };

public:
	Mesh(std::vector<Vertex> &&vertices, std::vector<Index> &&indices);
	Mesh(const Mesh &other) = delete;
	Mesh(Mesh &&other);
	Mesh& operator=(const Mesh &other) = delete;
	Mesh& operator=(Mesh &&other) = delete;
	~Mesh();
	void draw();

private:
	std::vector<Vertex> m_vertices;
	std::vector<Index> m_indices;
	GLuint m_VAO;
	GLuint m_EBO;
	GLuint m_VBO;
};

};
#endif
