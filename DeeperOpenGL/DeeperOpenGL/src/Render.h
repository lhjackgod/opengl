#pragma once
#include "Shader.h"
#include "OpenGLVertexArray.h"
#include <glad/glad.h>
class Render {
public:
	inline static void BeginScene(Shader& shader, OpenGLVertexArray& openglVertexArray) {
		openglVertexArray.Bind();
		shader.use();
	}
	inline static void RenderScene(GLsizei count) {
		glDrawArrays(GL_TRIANGLES, 0, count);
	}
};