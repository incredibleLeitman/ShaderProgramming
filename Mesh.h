#pragma once
#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Mesh
{
private:
	unsigned int m_VBO, m_VAO;
	int m_count;

public:
	Mesh (float *data, int count)
	{
		//m_count = count;
		m_count = 3 * count * sizeof(float);

		glGenBuffers(1, &m_VBO);
		glGenVertexArrays(1, &m_VAO);

		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBindVertexArray(m_VAO);

		// copy vertices to buffer
		glBufferData(GL_ARRAY_BUFFER, m_count, data, GL_STATIC_DRAW);

		// the vertex coordinates
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		/*/ TODO: add other data...
		// the normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));

		// the texture coordinates
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FLOAT, 11 * sizeof(float), (void*)(6 * sizeof(float)));

		// the tangent coordinates
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FLOAT, 11 * sizeof(float), (void*)(8 * sizeof(float)));*/

		// unbind
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	};

	~Mesh ()
	{
		glDeleteVertexArrays(1, &m_VAO);
		glDeleteBuffers(1, &m_VBO);
	}

	void draw (GLenum mode) const
	{
		glBindVertexArray(m_VAO);
		glDrawArrays(mode, 0, m_count);
	}
};
#endif // MESH_H