#pragma once
#include <glad/glad.h>
class Shader
{
public:
	Shader(const std::string& vf, const std::string& fv);
	void use();
	template<typename T>
	void SetValue(const std::string& name, const T& value);
	
private:
	unsigned int m_RendererID;
};
