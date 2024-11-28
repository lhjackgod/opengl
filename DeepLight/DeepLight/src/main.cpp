#include <iostream>
#include <string>
#include <vector>
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
void renderQuad();
static void RenderScene(Shader& shaderProgram);
static void bindTexture(uint32_t id, size_t slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, id);
}
uint32_t GetTexture(const std::string& tex, const bool& is_S);
struct UserData {
    float exposure = 1.0f;
    bool useHight = false;
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
                u_Data->exposure += 0.1f;
            }
        }
        else if (key == GLFW_KEY_L)
        {
            if (action == GLFW_PRESS)
            {
                u_Data->exposure -= 0.1f;
            }
        }
        if (key == GLFW_KEY_P)
        {
            if (action == GLFW_PRESS)
            {
                u_Data->useHight ^= 1;
            }
        }
    });
    
    Shader m_CubeShader("src/shader/hdr/Cube.vs", "src/shader/hdr/Cube.fs");
    Shader m_debugLightquad("src/shader/hdr/light.vs", "src/shader/hdr/light.fs");
    Shader m_Result("src/shader/hdr/hdr.vs", "src/shader/hdr/hdr.fs");
    m_Result.use();
    m_Result.SetValue("hdrBuffer", 0);

    uint32_t woodTex = GetTexture("src/resource/wood.png", true);

    float lastTime = static_cast<float>(glfwGetTime());

    uint32_t ubo;
    glGenBuffers(1, &ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, 2, ubo, 0, sizeof(glm::mat4) * 2);

 
    uint32_t hdrfbo, hdrTex, depthTex;
    glGenFramebuffers(1, &hdrfbo);
    glBindFramebuffer(GL_FRAMEBUFFER, hdrfbo);
    glGenTextures(1, &hdrTex);
    glGenRenderbuffers(1, &depthTex);
    glBindTexture(GL_TEXTURE_2D, hdrTex);
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREENWIDTH, SCREENHEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hdrTex, 0);
    }
    glBindRenderbuffer(GL_RENDERBUFFER, depthTex);
    {
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREENWIDTH, SCREENHEIGHT);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthTex);
    }
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << " frambuffer wrong \n";
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    //lightPos
    std::vector<glm::vec3> m_LightPos;
    m_LightPos.push_back(glm::vec3(0.0f, 0.0f, 49.5f)); // back light
    m_LightPos.push_back(glm::vec3(-1.4f, -1.9f, 9.0f));
    m_LightPos.push_back(glm::vec3(0.0f, -1.8f, 4.0f));
    m_LightPos.push_back(glm::vec3(0.8f, -1.7f, 6.0f));
    //LightColor
    std::vector<glm::vec3> m_LightColor;
    m_LightColor.push_back(glm::vec3(200.0f, 200.0f, 200.0f));
    m_LightColor.push_back(glm::vec3(0.1f, 0.0f, 0.0f));
    m_LightColor.push_back(glm::vec3(0.0f, 0.0f, 0.2f));
    m_LightColor.push_back(glm::vec3(0.0f, 0.1f, 0.0f));

    while (!glfwWindowShouldClose(window))
    {
        float nowTime = static_cast<float>(glfwGetTime());
        ProcessInput(window, nowTime - lastTime);
        lastTime = nowTime;
        glBindFramebuffer(GL_FRAMEBUFFER, hdrfbo);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 Matrix[2]{
            camera.GetViewMatrix(),
            camera.GetPerspectMatrix((float)SCREENWIDTH / (float)SCREENHEIGHT)
        };

        glBindBuffer(GL_UNIFORM_BUFFER, ubo);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4) * 2, glm::value_ptr(Matrix[0]));
        m_CubeShader.use();
        m_CubeShader.SetValue("uLightNums", (int) m_LightPos.size());
        for (int i = 0; i < m_LightPos.size(); i++)
        {
            m_CubeShader.SetValue("uLightPosArray[" + std::to_string(i) + "]", m_LightPos[i]);
            m_CubeShader.SetValue("uLightPosColorArray[" + std::to_string(i) + "]", m_LightColor[i]);
        }
        m_CubeShader.SetValue("uCameraPos", camera.GetPosition());
        m_CubeShader.SetValue("u_DiffuseTexture", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, woodTex);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 25.0));
        model = glm::scale(model, glm::vec3(2.5f, 2.5f, 27.5f));
        m_CubeShader.SetValue("model", model);
        m_CubeShader.SetValue("inverNormal", 1);
        RenderScene(m_CubeShader);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_Result.use();
        m_Result.SetValue("exposure", userData.exposure);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, hdrTex);
        renderQuad();

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
uint32_t GetTexture(const std::string& tex, const bool& is_S)
{
    uint32_t texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    int width, height, channel;
    stbi_set_flip_vertically_on_load(true);
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
        internalFormat = (is_S ? GL_SRGB : GL_RGB);
        break;
    case 4:
        dataFormat = GL_RGBA;
        internalFormat = (is_S ? GL_SRGB_ALPHA : GL_RGBA);
        break;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
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
            -1.0f, -1.0f, -1.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 1.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 1.0f, 1.0f, 1.0f,

            1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, -1.0f, 0.0f, 1.0f };
        size = 6;
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

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void RenderScene(Shader& shaderProgram)
{
    //cube
    int count = 0;
    VertexArray cubeVertex;
    RenderData(cubeVertex, count, (int)RENDERTYPE::CUBE);
    cubeVertex.Bind();
    glDrawArrays(GL_TRIANGLES, 0, count);
}


