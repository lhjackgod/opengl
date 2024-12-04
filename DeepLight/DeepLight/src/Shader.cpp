#include <iostream>

#include <fstream>
#include <sstream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
Shader::Shader(const std::string& vf, const std::string& fv)
{
	std::ifstream vfile, ffile;
    vfile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    ffile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    vfile.open(vf);
    ffile.open(fv);

    std::stringstream vstream, fstream;
    vstream << vfile.rdbuf();
    fstream << ffile.rdbuf();

    std::string v_str_code = vstream.str();
    std::string f_str_code = fstream.str();

    const char* v_code = v_str_code.c_str();
    const char* f_code = f_str_code.c_str();

    uint32_t v_shader, f_shader;
    v_shader = glCreateShader(GL_VERTEX_SHADER);
    f_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(v_shader, 1, &v_code, nullptr);
    glShaderSource(f_shader, 1, &f_code, nullptr);

    glCompileShader(v_shader);
    glCompileShader(f_shader);

    int v_success, f_success;
    glGetShaderiv(v_shader, GL_COMPILE_STATUS, &v_success);
    glGetShaderiv(f_shader, GL_COMPILE_STATUS, &f_success);

    if(!v_success)
    {
        int maxCount;
        glGetShaderiv(v_shader, GL_INFO_LOG_LENGTH, &maxCount);
        std::vector<GLchar> v_info(maxCount);
        glGetShaderInfoLog(v_shader, maxCount, &maxCount, &v_info[0]);

        std::cout<< vf << "has compile error" << "\t" << v_info.data() << std::endl;
    }
    if(!f_success)
    {
        int maxCount;
        glGetShaderiv(f_shader, GL_INFO_LOG_LENGTH, &maxCount);
        std::vector<GLchar> f_info(maxCount);
        glGetShaderInfoLog(f_shader, maxCount, &maxCount, &f_info[0]);

        std::cout<< fv << "has compile error" << "\t" << f_info.data() << std::endl;
    }

    uint32_t program = glCreateProgram();
    glAttachShader(program, v_shader);
    glAttachShader(program, f_shader);
    glLinkProgram(program);

    m_RendererID = program;
    glDetachShader(program, v_shader);
    glDetachShader(program, f_shader);
    glDeleteShader(v_shader);
    glDeleteShader(f_shader);
}
Shader::Shader(const std::string& vf, const std::string& gv, const std::string& fv)
{
    std::ifstream vfile, ffile, gfile;
    vfile.exceptions(std::ifstream::badbit | std::ifstream::failbit);
    ffile.exceptions(std::ifstream::badbit | std::ifstream::failbit);
    gfile.exceptions(std::ifstream::badbit | std::ifstream::failbit);

    vfile.open(vf);
    ffile.open(fv);
    gfile.open(gv);

    std::stringstream vss, fss, gss;
    std::string v_str, f_str, g_str;
    vss << vfile.rdbuf();
    fss << ffile.rdbuf();
    gss << gfile.rdbuf();

    v_str = vss.str(), f_str = fss.str(), g_str = gss.str();
    const GLchar* v_C;
    const GLchar* f_C;
    const GLchar* g_C;
    v_C = v_str.c_str(), f_C = f_str.c_str(), g_C = g_str.c_str();
    uint32_t v_Shader, f_Shader, g_Shader;
    {
        v_Shader = glCreateShader(GL_VERTEX_SHADER);
        f_Shader = glCreateShader(GL_FRAGMENT_SHADER);
        g_Shader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(v_Shader, 1, &v_C, nullptr);
        glShaderSource(f_Shader, 1, &f_C, nullptr);
        glShaderSource(g_Shader, 1, &g_C, nullptr);
    }
    {
        glCompileShader(v_Shader);
        glCompileShader(f_Shader);
        glCompileShader(g_Shader);
    }
    {
        int successShader;
        glGetShaderiv(v_Shader, GL_COMPILE_STATUS, &successShader);
        if (!successShader)
        {

            int maxSize;
            glGetShaderiv(v_Shader, GL_INFO_LOG_LENGTH, &maxSize);
            std::vector<GLchar> info(maxSize);
            glGetShaderInfoLog(v_Shader, maxSize, &maxSize, &info[0]);
            std::cout << vf << "has compile error" << "\t" << info.data() << std::endl;
        }

        glGetShaderiv(f_Shader, GL_COMPILE_STATUS, &successShader);
        if (!successShader)
        {
            int maxSize;
            glGetShaderiv(f_Shader, GL_INFO_LOG_LENGTH, &maxSize);
            std::vector<GLchar> info(maxSize);
            glGetShaderInfoLog(f_Shader, maxSize, &maxSize, &info[0]);
            std::cout << fv << "has compile error" << "\t" << info.data() << std::endl;
        }

        glGetShaderiv(g_Shader, GL_COMPILE_STATUS, &successShader);
        if (!successShader)
        {
            int maxSize;
            glGetShaderiv(g_Shader, GL_INFO_LOG_LENGTH, &maxSize);
            std::vector<GLchar> info(maxSize);
            glGetShaderInfoLog(g_Shader, maxSize, &maxSize, &info[0]);
            std::cout << gv << "has compile error" << "\t" << info.data() << std::endl;
        }
    }
    uint32_t program;
    program = glCreateProgram();
    glAttachShader(program, v_Shader);
    glAttachShader(program, f_Shader);
    glAttachShader(program, g_Shader);

    glLinkProgram(program);


    m_RendererID = program;

    glDetachShader(program, v_Shader);
    glDetachShader(program, f_Shader);
    glDetachShader(program, g_Shader);

    glDeleteShader(f_Shader);
    glDeleteShader(v_Shader);
    glDeleteShader(g_Shader);
}
void Shader::use()
{
    glUseProgram(m_RendererID);
}

template<>
void Shader::SetValue<glm::vec3>(const std::string& name, const glm::vec3& value)
{
    glUniform3fv(glGetUniformLocation(m_RendererID, name.c_str()),1, glm::value_ptr(value));
}
template<>
void Shader::SetValue<glm::mat4>(const std::string& name, const glm::mat4& value)
{
    glUniformMatrix4fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, GL_FALSE ,glm::value_ptr(value));
}
template<>
void Shader::SetValue<int>(const std::string& name, const int& value)
{
    glUniform1i(glGetUniformLocation(m_RendererID, name.c_str()), value);
}
template<>
void Shader::SetValue<float>(const std::string& name, const float& value)
{
    glUniform1f(glGetUniformLocation(m_RendererID, name.c_str()), value);
}
template<>
void Shader::SetValue<bool>(const std::string& name, const bool& value)
{
    glUniform1i(glGetUniformLocation(m_RendererID, name.c_str()), value);
}
template<>
void Shader::SetValue<uint32_t>(const std::string& name, const uint32_t& value)
{
    glUniform1i(glGetUniformLocation(m_RendererID, name.c_str()), (int)value);
}