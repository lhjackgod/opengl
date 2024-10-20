#include <glad/glad.h>
#include <iostream>
#include <GLFW/glfw3.h>
#include <sstream>
#include <fstream>
#include <filesystem>
int main(){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow * window = glfwCreateWindow(800, 600, "learOpengl", NULL, NULL);
    if(window == nullptr){
        std::cout<<"filed"<<std::endl;
    }
    glfwMakeContextCurrent(window);
    if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)){
        std::cout<<"filed"<<std::endl;
    }
    glViewport(0, 0, 800, 600);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height){glViewport(0, 0, width, height);});
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    float vertices[] = {
    // 位置              // 颜色
     0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // 右下
    -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // 左下
     0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // 顶部
};
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float)*3));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    std::string vertexstr;
    std::ifstream vertexFile("./src/shader/shader_lesson/vertex.glsl");
    std::stringstream vertexstream;
    vertexstream << vertexFile.rdbuf();
    vertexstr = vertexstream.str();
    const char* verSource = vertexstr.c_str();
    unsigned int vershader;
    vershader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vershader, 1, &verSource, NULL);
    glCompileShader(vershader);

    std::string fragstr;
    std::ifstream fragfile("./src/shader/shader_lesson/fragment.glsl");
    std::stringstream fragstream;
    fragstream << fragfile.rdbuf();
    fragstr = fragstream.str();
    const char* fragmentSource = fragstr.c_str();

    unsigned int fragshader;
    fragshader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragshader, 1, &fragmentSource, NULL);
    glCompileShader(fragshader);

    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vershader);
    glAttachShader(shaderProgram, fragshader);
    glLinkProgram(shaderProgram);

    while(!glfwWindowShouldClose(window)){
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        float timeValue = glfwGetTime();
        float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
        int vertecColorLoaction = glGetUniformLocation(shaderProgram, "outColor");
        glUseProgram(shaderProgram);
        glUniform4f(vertecColorLoaction, 0.0f, greenValue, 0.0f, 1.0f);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();

    return 0;
}