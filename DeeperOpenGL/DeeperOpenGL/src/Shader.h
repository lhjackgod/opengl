#pragma once
#include <iostream>
#include <sstream>
#include <fstream>
#include<glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
class Shader
{
public:
	Shader(const std::string& vfile, const std::string& ffile){
		std::ifstream VerFile, FragFile;
		VerFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		FragFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		VerFile.open(vfile);
		FragFile.open(ffile);
		std::stringstream verss,fragss;
		verss << VerFile.rdbuf();
		fragss << FragFile.rdbuf();
		std::string verstr = verss.str();
		std::string fragstr = fragss.str();
		const GLchar* verstr_c = verstr.c_str();
		const GLchar* fragstr_c = fragstr.c_str();

		GLuint program, verShader, fragShader;
		verShader = glCreateShader(GL_VERTEX_SHADER);
		fragShader = glCreateShader(GL_FRAGMENT_SHADER);
		program = glCreateProgram();
		glShaderSource(verShader, 1, &verstr_c, nullptr);
		glShaderSource(fragShader, 1, &fragstr_c, nullptr);

		glCompileShader(verShader);
		glCompileShader(fragShader);

		int successver, successfrag, successprogram;
		glGetShaderiv(verShader, GL_COMPILE_STATUS, &successver);
		if (!successver) {
			std::cout << "ver error" << std::endl;
		}
		glGetShaderiv(fragShader, GL_COMPILE_STATUS, &successfrag);
		if (!successfrag) {
			std::cout << "frag error" << std::endl;
		}
		glAttachShader(program, verShader);
		glAttachShader(program, fragShader);
		glLinkProgram(program);
		glGetProgramiv(program, GL_LINK_STATUS, &successprogram);
		if (!successprogram) {
			int maxCount;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxCount);
			std::vector<GLchar> infoLog(maxCount);
			glGetProgramInfoLog(program, maxCount, &maxCount, &infoLog[0]);
			
			std::cout << infoLog.data() << std::endl;
		}
		m_RendererID = program;
		glDetachShader(program,verShader);
		glDetachShader(program,fragShader);
		glDeleteShader(verShader);
		glDeleteShader(fragShader);
	}
	void setInt(const std::string& name, int value);
	void setMat4(const std::string& name, glm::mat4 value);
	void setVec4(const std::string& name, glm::vec4 value);
	void setVec3(const std::string& name, glm::vec3 value);
	inline void use() { glUseProgram(m_RendererID); }
private:
	uint32_t m_RendererID;

};

