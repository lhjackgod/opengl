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
	Shader(const std::string& vfile, const std::string& ffile, const std::string& gfile = ""){
		std::ifstream VerFile, FragFile, GeoFile;
		VerFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		FragFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		GeoFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		VerFile.open(vfile);
		FragFile.open(ffile);
		std::stringstream verss,fragss;
		verss << VerFile.rdbuf();
		fragss << FragFile.rdbuf();
		std::string verstr = verss.str();
		std::string fragstr = fragss.str();
		const GLchar* verstr_c = verstr.c_str();
		const GLchar* fragstr_c = fragstr.c_str();

		GLuint program, verShader, fragShader, geoShader;

		if(!gfile.empty())
		{
			GeoFile.open(gfile);
			std::stringstream geoss;
			geoss << GeoFile.rdbuf();
			std::string geostr = geoss.str();
			const GLchar* geostr_c = geostr.c_str();

			geoShader = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geoShader, 1, &geostr_c, nullptr);
			glCompileShader(geoShader);
			int successgeoshader;
			glGetShaderiv(geoShader, GL_COMPILE_STATUS, &successgeoshader);
			if(!successgeoshader)
			{
				int maxCount;
				glGetShaderiv(geoShader, GL_INFO_LOG_LENGTH, & maxCount);
				std::vector<GLchar> info(maxCount);
				glGetShaderInfoLog(geoShader, maxCount, &maxCount, &info[0]);

				std::cout<<info.data()<<std::endl;
			}
		}
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
			int maxCount;
			glGetShaderiv(verShader, GL_INFO_LOG_LENGTH, &maxCount);
			std::vector<GLchar> info(maxCount);
			glGetShaderInfoLog(verShader, maxCount, &maxCount, &info[0]);
			std::cout<<vfile<<"\t";
			std::cout << info.data() << std::endl;
		}
		glGetShaderiv(fragShader, GL_COMPILE_STATUS, &successfrag);
		if (!successfrag) {
			std::cout << "frag error" << std::endl;
		}
		glAttachShader(program, verShader);
		glAttachShader(program, fragShader);
		if(!gfile.empty())
		{
			glAttachShader(program, geoShader);
		}
		glLinkProgram(program);
		glGetProgramiv(program, GL_LINK_STATUS, &successprogram);
		if (!successprogram) {
			int maxCount;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxCount);
			std::vector<GLchar> infoLog(maxCount);
			glGetProgramInfoLog(program, maxCount, &maxCount, &infoLog[0]);
			
			std::cout << infoLog.data() << std::endl;
		}
		glDetachShader(program,verShader);
		glDetachShader(program,fragShader);
		glDeleteShader(verShader);
		glDeleteShader(fragShader);
		if(!gfile.empty())
		{
			glDetachShader(program, geoShader);
			glDeleteShader(geoShader);
		}
		m_RendererID = program;
	}
	void setInt(const std::string& name, int value);
	void setMat4(const std::string& name, glm::mat4 value);
	void setVec4(const std::string& name, glm::vec4 value);
	void setVec3(const std::string& name, glm::vec3 value);
	inline void use() { glUseProgram(m_RendererID); }
private:
	uint32_t m_RendererID;

};

