#include "includes.h"

#include "mesh.h"


Mesh::Mesh() :
	Component(Component::MESH),
	m_vertices(),
	m_indices()
{
	const float TAU = 6.2831853071;
	const float PI    = 3.14159265359;
	unsigned int xSegments = 30;
	unsigned int ySegments = 30;
	for (unsigned int y = 0; y <= ySegments; ++y)
        {
            for (unsigned int x = 0; x <= xSegments; ++x)
            {
                float xSegment = (float)x / (float)xSegments;
                float ySegment = (float)y / (float)ySegments;
                float xPos = std::cos(xSegment * TAU) * std::sin(ySegment * PI); // TAU is 2PI
                float yPos = std::cos(ySegment * PI);
                float zPos = std::sin(xSegment * TAU) * std::sin(ySegment * PI);

                m_vertices.push_back({
			{xPos, yPos, zPos},
			{xPos, yPos, zPos},
			{xSegment, ySegment}
		});
		m_indices.push_back({
			(y + 1) * (xSegments + 1) + x,
			y * (xSegments + 1) + x,
			y * (xSegments + 1) + x + 1
		});
		m_indices.push_back({
			(y + 1) * (xSegments + 1) + x,
			y * (xSegments + 1) + x + 1,
			(y + 1) * (xSegments + 1) + x + 1
		});
	    }
        }

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

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_EBO);
	glDeleteBuffers(1, &m_VBO);
}

void Mesh::draw(int instances_count)
{
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

    glDrawElementsInstanced(GL_TRIANGLES, m_indices.size() * sizeof(GLuint), GL_UNSIGNED_INT, 0, instances_count);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Mesh::reload()
{
}
