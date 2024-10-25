#pragma once
#include <glad/glad.h>
#include <fstream>
#include <sstream>
class Shader
{
public:
	Shader(const char* vertexPath, const char* fragmentPath) {
		std::string versource, fragsource;
		std::fstream verfile, fragfile;
		verfile.exceptions(std::ifstream::badbit | std::ifstream::failbit);
		fragfile.exceptions(std::ifstream::badbit | std::ifstream::failbit);
		verfile.open(vertexPath);
		fragfile.open(fragmentPath);
		std::stringstream verss;
		std::stringstream fragss;
		verss << verfile.rdbuf();
		fragss <<fragfile.rdbuf();
		versource = verss.str();
		fragsource = fragss.str();
		verfile.close();
		fragfile.close();
		const char* verc = versource.c_str();
		const char* fragc = fragsource.c_str();
		unsigned int vertexID, fragID;
		vertexID = glCreateShader(GL_VERTEX_SHADER);
		fragID = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(vertexID, 1, &verc, NULL);
		glShaderSource(fragID, 1, &fragc, NULL);
		glCompileShader(vertexID);
		glCompileShader(fragID);
		int success;
		glGetShaderiv(fragID, GL_COMPILE_STATUS, &success);
		if (!success) {
			std::cout << "error v" << std::endl;
		}
		ShaderProgramID = glCreateProgram();
		glAttachShader(ShaderProgramID, vertexID);
		glAttachShader(ShaderProgramID, fragID);
		glLinkProgram(ShaderProgramID);
		glGetProgramiv(ShaderProgramID, GL_LINK_STATUS, &success);
		if (!success) {
			std::cout << "program" << std::endl;
		}
		glDeleteShader(vertexID);
		glDeleteShader(fragID);
	}
	void use() {
		glUseProgram(ShaderProgramID);
	}

	unsigned int ShaderProgramID;
};

