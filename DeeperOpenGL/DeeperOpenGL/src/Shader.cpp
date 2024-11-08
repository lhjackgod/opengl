#include "Shader.h"
#include "glm/gtc/type_ptr.hpp"
void Shader::setInt(const std::string& name, int value)
{
	glUniform1i(glGetUniformLocation(m_RendererID, name.c_str()), value);
}
void Shader::setMat4(const std::string& name, glm::mat4 value)
{
	glUniformMatrix4fv(glGetUniformLocation(m_RendererID,name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}
