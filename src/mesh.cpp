#include "includes.h"

#include "mesh.h"
#include "timer.h"

namespace sge
{

Mesh::Mesh(std::vector<Vertex> &&vertices, std::vector<Index> &&indices) :
    m_VAO{0}, m_VBO{0}, m_EBO{0},
	m_vertices{std::move(vertices)},
	m_indices{std::move(indices)}
{
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);
	
	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), m_vertices.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &m_EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(Index), m_indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(glm::vec3));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2*sizeof(glm::vec3)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}

Mesh::Mesh(Mesh &&other) :
    m_VAO{0}, m_VBO{0}, m_EBO{0}
{
    std::swap(m_vertices, other.m_vertices);
    std::swap(m_indices, other.m_indices);
    std::swap(m_VAO, other.m_VAO);
    std::swap(m_VBO, other.m_VBO);
    std::swap(m_EBO, other.m_EBO);
}

Mesh::~Mesh()
{
    glDeleteBuffers(1, &m_EBO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteVertexArrays(1, &m_VAO);
}

void Mesh::draw()
{
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glDrawElements(GL_TRIANGLES, m_indices.size() * 3, GL_UNSIGNED_INT, 0);
}

};
