#include "Shader.h"

void Shader::setVec3(const char* name, float a, float b, float c)
{
	glUniform3f(glGetUniformLocation(ShaderProragmID, name), a, b, c);
}

void Shader::setVec3(const char* name, glm::vec3 a)
{
	glUniform3fv(glGetUniformLocation(ShaderProragmID, name), 1, glm::value_ptr(a));
}

void Shader::setMatrix4(const char* name, glm::mat4 matrix)
{
	glUniformMatrix4fv(glGetUniformLocation(ShaderProragmID, name), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::setInt(const char* name, int a)
{
	glUniform1i(glGetUniformLocation(ShaderProragmID, name), a);
}

void Shader::setFloat(const char* name, float a)
{
	glUniform1f(glGetUniformLocation(ShaderProragmID, name), a);
}
