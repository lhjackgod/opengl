#pragma once
#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <sstream>
class Shader
{
public:
    unsigned int ID;
    Shader(const char* vershaderPath, const char* fragmentShaderPath){
        std::string verShaderSrc, fragmentShaderSrc;
        std::fstream verFile(vershaderPath);
        std::stringstream verSS;
        verSS << verFile.rdbuf();
        verShaderSrc = verSS.str();
        const char* verShaderSource = verShaderSrc.c_str();
        unsigned int verShaderID;
        verShaderID = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(verShaderID, 1, &verShaderSource, NULL);
        glCompileShader(verShaderID);
        int success;
        
        std::fstream fragFile(fragmentShaderPath);
        std::stringstream fragSS;
        fragSS << fragFile.rdbuf();
        fragmentShaderSrc = fragSS.str();
        const char* fragmentSource = fragmentShaderSrc.c_str();
        unsigned int fragmentID;
        fragmentID = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentID, 1, &fragmentSource, NULL);
        glCompileShader(fragmentID);

        ID = glCreateProgram();
        glAttachShader(ID, verShaderID);
        glAttachShader(ID, fragmentID);
        glLinkProgram(ID);
        
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if(!success){
            std::cout<<"wrong";
        }
        glDeleteShader(verShaderID);
        glDeleteShader(fragmentID);

    }
    void use(){
        glUseProgram(ID);
    }
};