#include <iostream>
#include "Shader.h"
#include "Camera.h"
#include "VertexArray.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
int SCREENWIDTH = 800;
int SCREENHEIGHT = 800;
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), (float)SCREENWIDTH / (float)SCREENHEIGHT);
double nowPosX = SCREENWIDTH / 2;
double nowPosY = SCREENHEIGHT / 2;
bool fistMouseMove = true;
void ProcessInput(GLFWwindow* window, float deltaTime);
void MouseMove(GLFWwindow* window, double xpos, double ypos);
void MouseScroll(GLFWwindow* window, double xoffset, double yoffset);
void RenderData(VertexArray& outver ,int& size, int type);
static void RenderScene(Shader& shaderProgram);
static void bindTexture(uint32_t id, size_t slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, id);
}
uint32_t GetTexture(const std::string& tex);
struct UserData {
    int useBinPhong = 1;
    bool usequad = false;
}userData;
enum class RENDERTYPE
{
    CUBE = 0,
    QUAD,
    PLANE,
};
static uint32_t GetWhiteTexture()
{
    unsigned char* whiteData = new unsigned char[1 * 1 * 4];

    whiteData[0] = 255;  // 红色通道
    whiteData[1] = 255;  // 绿色通道
    whiteData[2] = 255;  // 蓝色通道
    whiteData[3] = 255;  // 透明度通道
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whiteData);
    delete[] whiteData;

    // 返回创建好的纹理ID，以便后续在渲染等操作中使用
    return textureID;
}
int main()
{
    

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCREENWIDTH, SCREENHEIGHT, "deep light", NULL, NULL);
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "error" << std::endl;
        return -1;
    }
    glViewport(0, 0, SCREENWIDTH, SCREENHEIGHT);
    glfwSetCursorPosCallback(window, MouseMove);
    glfwSetScrollCallback(window, MouseScroll);
    glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    });
    glfwSetWindowUserPointer(window, &userData);
    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        UserData* u_Data = (UserData*)glfwGetWindowUserPointer(window);
        if (key == GLFW_KEY_B)
        {
            if (action == GLFW_PRESS)
            {
                u_Data->useBinPhong ^= 1;
            }
        }
        if (key == GLFW_KEY_P)
        {
            if (action == GLFW_PRESS)
            {
                u_Data->usequad ^= 1;
            }
        }
    });
    

    Shader m_SceneShader("src/shader/floor.vs", "src/shader/floor.fs");
    Shader m_RenderDepth("src/shader/GenDepth.vs", "src/shader/GenDepth.fs");
    Shader m_DebugShader("src/shader/debug.vs", "src/shader/debug.fs");
    int m_planeSize;
    VertexArray m_plane;
    RenderData(m_plane, m_planeSize, (int)RENDERTYPE::QUAD);
    uint32_t woodPng = GetTexture("src/resource/wood.png");
   

    float lastTime = static_cast<float>(glfwGetTime());

    uint32_t ubo;
    glGenBuffers(1, &ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, 2, ubo, 0, sizeof(glm::mat4) * 2);
    
    uint32_t depthFbo, depthTex;
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    {
        glGenFramebuffers(1, &depthFbo);
        glBindFramebuffer(GL_FRAMEBUFFER, depthFbo);
        
        glGenTextures(1, &depthTex);
        glBindTexture(GL_TEXTURE_2D, depthTex);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);
    //-3.29274 3.52926 -0.580889
    //0.871536 -0.45399 0.185251
    while (!glfwWindowShouldClose(window))
    {
        float nowTime = static_cast<float>(glfwGetTime());
        ProcessInput(window, nowTime - lastTime);
        lastTime = nowTime;

        glm::mat4 LightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        glm::mat4 LightOrtho = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 7.5f);
        glBindFramebuffer(GL_FRAMEBUFFER, depthFbo);
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_DEPTH_BUFFER_BIT);
        m_RenderDepth.use();
        m_RenderDepth.SetValue("lightSpaceMatrix", LightOrtho * LightView);

        RenderScene(m_RenderDepth);
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, SCREENWIDTH, SCREENHEIGHT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //uniform
        glBindBuffer(GL_UNIFORM_BUFFER, ubo);
        glm::mat4 Matrix[2]{
            camera.GetViewMatrix(),
            camera.GetPerspectMatrix((float) SCREENWIDTH / (float) SCREENWIDTH)
        };
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4) * 2, glm::value_ptr(Matrix[0]));

        m_SceneShader.use();
        m_SceneShader.SetValue("LightOrtho", LightOrtho * LightView);
        m_SceneShader.SetValue("uCameraPos", camera.GetPosition());
        m_SceneShader.SetValue("uLightPos", lightPos);

        m_SceneShader.SetValue("uTexture", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, woodPng);

        m_SceneShader.SetValue("depthMap", 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthTex);

        RenderScene(m_SceneShader);
        /*glViewport(0, 0, SCREENWIDTH, SCREENHEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        RenderScene(m_DebugShader);*/

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
void ProcessInput(GLFWwindow* window, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.KeyClick(KEYTYPE::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.KeyClick(KEYTYPE::RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.KeyClick(KEYTYPE::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.KeyClick(KEYTYPE::BACK, deltaTime);
}
void MouseMove(GLFWwindow* window, double xpos, double ypos)
{
    if (fistMouseMove)
    {
        nowPosX = xpos;
        nowPosY = ypos;
        fistMouseMove = false;
    }
    camera.MouseMove(static_cast<float>(xpos - nowPosX), static_cast<float>(nowPosY - ypos));
    nowPosX = xpos;
    nowPosY = ypos;
}
void MouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.MouseScroll(static_cast<float>(yoffset));
}
uint32_t GetTexture(const std::string& tex)
{
    uint32_t texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    int width, height, channel;
    stbi_uc* data = stbi_load(tex.c_str(), &width, &height, &channel, 0);
    if (!data)
    {
        std::cout << "no tex" << std::endl;
    }
    GLenum dataFormat, internalFormat;
    switch (channel)
    {
    case 1:
        dataFormat = GL_RED;
        internalFormat = GL_RED;
        break;
    case 3:
        dataFormat = GL_RGB;
        internalFormat = GL_SRGB;
        break;
    case 4:
        dataFormat = GL_RGBA;
        internalFormat = GL_SRGB_ALPHA;
        break;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);
    return texID;
}

void RenderData(VertexArray& outver, int& size, int type)
{
    switch ((RENDERTYPE)type)
    {
    case RENDERTYPE::CUBE:
    {
        float vertices[]{
            // back face
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            // right face
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
            // bottom face
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
            -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
            // top face
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
             1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
             1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
            -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
        };
        size = 36;
        outver.SetData(vertices, sizeof(vertices), 3);
        break;
    }
        
    case RENDERTYPE::QUAD:
    {
        float vertices[]{
            -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f, };
        size = 4;
        outver.SetData(vertices, sizeof(vertices), 1);
        break;
    }
    case RENDERTYPE::PLANE:
    {
        float planeVertices[] = {
            // positions            // normals         // texcoords
             25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
            -25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
            -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

             25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
            -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
             25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
        };
        size = 6;
        outver.SetData(planeVertices, sizeof(planeVertices),3);
        break;
    }
        
    }
}

void RenderScene(Shader& shaderProgram)
{
    //floor
    shaderProgram.use();
    glm::mat4 model = glm::mat4(1.0f);
    shaderProgram.SetValue("model", model);
    int count;
    VertexArray planeVertices;
    RenderData(planeVertices, count, (int)RENDERTYPE::PLANE);
    planeVertices.Bind();
    glDrawArrays(GL_TRIANGLES, 0, count);

    //cubes
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
    model = glm::scale(model, glm::vec3(0.5f));
    shaderProgram.SetValue("model", model);
    VertexArray cubeVertices;
    RenderData(cubeVertices, count, (int)RENDERTYPE::CUBE);
    cubeVertices.Bind();
    glDrawArrays(GL_TRIANGLES, 0, count);

    model = glm::mat4(1.0);
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
    model = glm::scale(model, glm::vec3(0.5f));
    shaderProgram.SetValue("model", model);
    VertexArray cubeVertices2;
    RenderData(cubeVertices2, count, (int)RENDERTYPE::CUBE);
    cubeVertices2.Bind();
    glDrawArrays(GL_TRIANGLES, 0, count);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
    model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
    model = glm::scale(model, glm::vec3(0.25));
    shaderProgram.SetValue("model", model);
    VertexArray cubeVertices3;
    RenderData(cubeVertices3, count, (int)RENDERTYPE::CUBE);
    cubeVertices3.Bind();
    glDrawArrays(GL_TRIANGLES, 0, count);
}
