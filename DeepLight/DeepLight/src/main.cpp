#include <iostream>
#include <string>
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
void RenderQuad();
static void RenderScene(Shader& shaderProgram);
static void bindTexture(uint32_t id, size_t slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, id);
}
uint32_t GetTexture(const std::string& tex);
struct UserData {
    float heightscale = 0.1f;
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
                u_Data->heightscale += 0.01f;
            }
        }
        else if (key == GLFW_KEY_L)
        {
            if (action == GLFW_PRESS)
            {
                u_Data->heightscale -= 0.01f;
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
    

    Shader m_quadShader("src/shader/quad.vs", "src/shader/quad.fs");

    
    uint32_t blockTex = GetTexture("src/resource/bricks2.jpg");
    uint32_t blockNormalTex = GetTexture("src/resource/bricks2_normal.jpg");
    uint32_t blockHeightTex = GetTexture("src/resource/bricks2_disp.jpg");

    m_quadShader.use();
    m_quadShader.SetValue("u_DiffuseTexture", 0);
    m_quadShader.SetValue("u_NormalTex", 1);
    m_quadShader.SetValue("u_HeightTex", 2);

    float lastTime = static_cast<float>(glfwGetTime());

    uint32_t ubo;
    glGenBuffers(1, &ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, 2, ubo, 0, sizeof(glm::mat4) * 2);
    

    glm::vec3 lightPos(0.5f, 1.0f, 0.3f);

  

    while (!glfwWindowShouldClose(window))
    {
        float nowTime = static_cast<float>(glfwGetTime());
        ProcessInput(window, nowTime - lastTime);
        lastTime = nowTime;
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 Matrix[2]{
            camera.GetViewMatrix(),
            camera.GetPerspectMatrix((float)SCREENWIDTH / (float)SCREENHEIGHT)
        };

        glBindBuffer(GL_UNIFORM_BUFFER, ubo);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4) * 2, glm::value_ptr(Matrix[0]));

        m_quadShader.use();
        m_quadShader.SetValue("uLightPos", lightPos);
        m_quadShader.SetValue("uCameraPos", camera.GetPosition());
        m_quadShader.SetValue("model", glm::mat4(1.0));
        m_quadShader.SetValue("heightScale", userData.heightscale);
        m_quadShader.SetValue("useHight", userData.useHight ? 1 : 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, blockTex);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, blockNormalTex);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, blockHeightTex);
        RenderQuad();
        
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
        internalFormat = GL_RGB;
        break;
    case 4:
        dataFormat = GL_RGBA;
        internalFormat = GL_RGBA;
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
void RenderQuad()
{
    if (!quadVAO)
    {
        glm::vec3 pos1(-1.0f, 1.0f, 0.0f);
        glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
        glm::vec3 pos3(1.0f, -1.0f, 0.0f);
        glm::vec3 pos4(1.0f, 1.0f, 0.0f);
        // texture coordinates
        glm::vec2 uv1(0.0f, 1.0f);
        glm::vec2 uv2(0.0f, 0.0f);
        glm::vec2 uv3(1.0f, 0.0f);
        glm::vec2 uv4(1.0f, 1.0f);

        //current face only has this local normal
        glm::vec3 nm(0.0f, 0.0f, 1.0f);

        glm::vec3 tangent1, bitangent1;
        glm::vec3 tangent2, bitangent2;

        glm::vec3 edge1 = pos2 - pos1;
        glm::vec3 edge2 = pos3 - pos1;

        glm::vec2 deltaUV1 = uv2 - uv1;
        glm::vec2 deltaUV2 = uv3 - uv1;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
        tangent1 = (deltaUV2.y * edge1 - deltaUV1.y * edge2) * f;
        bitangent1 = (-1.0f * deltaUV2.x * edge1 + deltaUV1.x * edge2) * f;

        //tangent2
        edge1 = pos3 - pos1;
        edge1 = pos4 - pos1;

        deltaUV1 = uv3 - uv1;
        deltaUV2 = uv4 - uv1;

        f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
        tangent2 = (deltaUV2.y * edge1 - deltaUV1.y * edge2) * f;
        bitangent2 = (-1.0f * deltaUV2.x * edge1 + deltaUV1.x * edge2) * f;

        float quadVertices[] = {
            pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
            pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
            pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

            pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
            pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
            pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
        };
        glGenVertexArrays(1, &quadVAO);
        uint32_t quadvbo;
        glBindVertexArray(quadVAO);
        glGenBuffers(1, &quadvbo);
        glBindBuffer(GL_ARRAY_BUFFER, quadvbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 14, (void*)(0));
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 14, (void*)(sizeof(float) * 3));
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 14, (void*)(sizeof(float) * 6));
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 14, (void*)(sizeof(float) * 8));
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 14, (void*)(sizeof(float) * 11));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void RenderScene(Shader& shaderProgram)
{
    //floor
    glm::mat4 model = glm::mat4(1.0f);
    shaderProgram.SetValue("model", model);
    VertexArray floorVertex;
    int count;
    RenderData(floorVertex, count, (int)RENDERTYPE::QUAD);
    floorVertex.Bind();
    glDrawArrays(GL_TRIANGLES, 0, count);
    return;

    //cube
    model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(5.0f));
    shaderProgram.SetValue("model", model);
    shaderProgram.SetValue("reverse_normals", 1);
    glDisable(GL_CULL_FACE);

    VertexArray planeVertices;
    RenderData(planeVertices, count, (int)RENDERTYPE::CUBE);
    planeVertices.Bind();
    glDrawArrays(GL_TRIANGLES, 0, count);
    shaderProgram.SetValue("reverse_normals", 0);
    glEnable(GL_CULL_FACE);
    //cubes
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(4.0f, -3.5f, 0.0));
    model = glm::scale(model, glm::vec3(0.5f));
    shaderProgram.SetValue("model", model);
    VertexArray cubeVertices;
    RenderData(cubeVertices, count, (int)RENDERTYPE::CUBE);
    cubeVertices.Bind();
    glDrawArrays(GL_TRIANGLES, 0, count);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 3.0f, 1.0));
    model = glm::scale(model, glm::vec3(0.75f));
    shaderProgram.SetValue("model", model);
    VertexArray cubeVertices2;
    RenderData(cubeVertices2, count, (int)RENDERTYPE::CUBE);
    cubeVertices2.Bind();
    glDrawArrays(GL_TRIANGLES, 0, count);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-3.0f, -1.0f, 0.0));
    model = glm::scale(model, glm::vec3(0.5f));
    shaderProgram.SetValue("model", model);
    VertexArray cubeVertices3;
    RenderData(cubeVertices3, count, (int)RENDERTYPE::CUBE);
    cubeVertices3.Bind();
    glDrawArrays(GL_TRIANGLES, 0, count);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.5f, 1.0f, 1.5));
    model = glm::scale(model, glm::vec3(0.5f));
    shaderProgram.SetValue("model", model);
    VertexArray cubeVertices4;
    RenderData(cubeVertices4, count, (int)RENDERTYPE::CUBE);
    cubeVertices4.Bind();
    glDrawArrays(GL_TRIANGLES, 0, count);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.5f, 2.0f, -3.0));
    model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
    model = glm::scale(model, glm::vec3(0.75f));
    shaderProgram.SetValue("model", model);
    VertexArray cubeVertices5;
    RenderData(cubeVertices5, count, (int)RENDERTYPE::CUBE);
    cubeVertices5.Bind();
    glDrawArrays(GL_TRIANGLES, 0, count);
}
