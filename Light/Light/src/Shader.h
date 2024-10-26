#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
class Shader
{
public:
	Shader(const char* verPath, const char* fragPath) {
		std::fstream verFile, fragmentFile;
		verFile.exceptions(std::ofstream::badbit | std::ofstream::failbit);
		fragmentFile.exceptions(std::ofstream::badbit | std::ofstream::failbit);
		verFile.open(verPath);
		fragmentFile.open(fragPath);
		std::stringstream verss, fragss;
		verss << verFile.rdbuf();
		fragss << fragmentFile.rdbuf();
		std::string v_s = verss.str();
		std::string f_s = fragss.str();
		const char* verSourc = v_s.c_str();
		const char* fragSourc = f_s.c_str();
		unsigned int verShaderID, fragmentShaderID;
		verShaderID = glCreateShader(GL_VERTEX_SHADER);
		fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(verShaderID, 1, &verSourc, NULL);
		glShaderSource(fragmentShaderID, 1, &fragSourc, NULL);
		glCompileShader(verShaderID);
		glCompileShader(fragmentShaderID);
		int success;
		glGetShaderiv(verShaderID, GL_COMPILE_STATUS, &success);
		
		ShaderProragmID = glCreateProgram();
		glAttachShader(ShaderProragmID, verShaderID);
		glAttachShader(ShaderProragmID, fragmentShaderID);
		glLinkProgram(ShaderProragmID);
		glGetProgramiv(ShaderProragmID, GL_LINK_STATUS, &success);
		if (!success) {
			std::cout << "jack" << std::endl;
		}
		glDeleteShader(verShaderID);
		glDeleteShader(fragmentShaderID);
	}
	unsigned int getShaderProgram() { return ShaderProragmID; }
	void use() { glUseProgram(ShaderProragmID); }
	void setVec3(const char* name, float a, float b, float c);
	void setVec3(const char* name, glm::vec3 a);
	void setMatrix4(const char* name, glm::mat4 matrix);
	void setInt(const char* name, int a);
	void setFloat(const char* name, float a);
private:
	unsigned int ShaderProragmID;
};

