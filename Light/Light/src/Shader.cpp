#include "Shader.h"

void Shader::setVec3(const char* name, float a, float b, float c)
{
	glUniform3f(glGetUniformLocation(ShaderProragmID, name), a, b, c);
}

void Shader::setMatrix4(const char* name, glm::mat4 matrix)
{
	glUniformMatrix4fv(glGetUniformLocation(ShaderProragmID, name), 1, GL_FALSE, glm::value_ptr(matrix));
}
