#include "OpenGLVertexArray.h"
#include <glad/glad.h>
OpenGLVertexArray::OpenGLVertexArray(float* vertices, uint32_t size, int type)
{
	glGenVertexArrays(1, &m_RendererID);
	glBindVertexArray(m_RendererID);
	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	switch (type)
	{
	case 0:
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		break;
	case 1:
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));
		break;
	case 2:
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(0));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(sizeof(float) * 3));
		break;
	}
	glBindVertexArray(0);
}

void OpenGLVertexArray::Bind()
{
	glBindVertexArray(m_RendererID);
}

uint32_t OpenGLVertexArray::GetVertexArray()
{
	return m_RendererID;
}
