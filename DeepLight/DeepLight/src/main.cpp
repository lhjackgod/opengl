#include <iostream>
#include <string>
#include <vector>
#include <random>
#include "Shader.h"
#include "Camera.h"
#include "VertexArray.h"
#include "model.h"
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
void renderCube();
static void RenderScene(Shader& shaderProgram, const uint32_t& secondTex);
static void bindTexture(uint32_t id, size_t slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, id);
}
uint32_t GetTexture(const std::string& tex, const bool& is_S);
struct UserData {
    float exposure = 1.0f;
    bool useBoolm = false;
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

    whiteData[0] = 255;  // ��ɫͨ��
    whiteData[1] = 255;  // ��ɫͨ��
    whiteData[2] = 255;  // ��ɫͨ��
    whiteData[3] = 255;  // ͸����ͨ��
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whiteData);
    delete[] whiteData;

    // ���ش����õ�����ID���Ա��������Ⱦ�Ȳ�����ʹ��
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
                u_Data->useBoolm ^= 1;
            }
        }
    });

    //uniform buffer
    uint32_t ubo;
    glGenBuffers(1, &ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, 2, ubo, 0, 2 * sizeof(glm::mat4));
    


    float lastTime = static_cast<float>(glfwGetTime());


    //model
    Model backobj("src/resource/backpack/backpack.obj");
    //shader
    Shader modelShader("src/shader/ssao/model.vs", "src/shader/ssao/model.fs");
    Shader roomShader("src/shader/ssao/room.vs", "src/shader/ssao/room.fs");
    Shader screenShader("src/shader/ssao/screen.vs", "src/shader/ssao/screen.fs");
    //ssao for judge the visibility
    Shader ssaoShader("src/shader/ssao/screen.vs", "src/shader/ssao/ssao.fs");
    Shader m_totalShader("src/shader/ssao/screen.vs", "src/shader/ssao/ssaoLight.fs");
    Shader m_ssaoBlurShader("src/shader/ssao/screen.vs", "src/shader/ssao/ssaoBlur.fs");

    uint32_t gBuffer;
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    uint32_t textureID[3];
    {
        glGenTextures(3, textureID);
        for (int i = 0; i < 3; i++)
        {
            glBindTexture(GL_TEXTURE_2D, textureID[i]);
            if (i == 2)
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCREENWIDTH, SCREENHEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
            }
            else
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREENWIDTH, SCREENHEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
            }
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T ,GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textureID[i], 0);
        }
    }
    auto&[gPosition, gNormal, gColor] = textureID;
    uint32_t rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCREENWIDTH, SCREENHEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    uint32_t coloAttachment[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    glDrawBuffers(3, coloAttachment);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    //ssao Frambuffer
    uint32_t ssaoFBO, ssaoTex, ssaoDebugTex;
    {
        glGenFramebuffers(1, &ssaoFBO);
        glGenTextures(1, &ssaoTex);
        glBindTexture(GL_TEXTURE_2D, ssaoTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCREENWIDTH, SCREENHEIGHT, 0, GL_RED, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoTex, 0);

        glGenTextures(1, &ssaoDebugTex);
        glBindTexture(GL_TEXTURE_2D, ssaoDebugTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREENWIDTH, SCREENHEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, ssaoDebugTex, 0);
        
        uint32_t colorattachments[2] {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
        glDrawBuffers(2, colorattachments);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    //random way
    std::default_random_engine engine;
    std::uniform_real_distribution<float> randNumber(0.0f, 1.0f);
    //texNoise
    std::vector<glm::vec3> texNoise;
    for(int i = 0; i < 16; i++)
    {
        glm::vec3 noise(randNumber(engine) * 2.0f - 1.0f, 
            randNumber(engine) * 2.0f - 1.0f,
            0.0f);
        texNoise.emplace_back(noise);  
    }
    //create Tex
    uint32_t texNoiseTex;
    {
        glGenTextures(1, &texNoiseTex);
        glBindTexture(GL_TEXTURE_2D, texNoiseTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &texNoise[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    //samples
    int kernelSize = 64;
    float radius = 0.5f;
    std::vector<glm::vec3> samples;
    for(int i = 0; i < kernelSize; i++)
    {
        glm::vec3 samplePoint = glm::vec3(
          randNumber(engine) * 2.0f - 1.0f,
          randNumber(engine) * 2.0f - 1.0f,
          randNumber(engine)
        );
        samplePoint = glm::normalize(samplePoint);
        //more random not only on the face
        samplePoint *= randNumber(engine);
        //then we want the samplepoint get more nums close the shader point
        //so we need to scale the samplePoint
        //but we not want all of the point inside a range
        auto ourlerp = [](float a, float b, float t){ return a + (b - a) * t; };
        float scale = (float) i / (float) kernelSize;
        scale = ourlerp(0.0, 1.0, scale * scale);
        samplePoint *= scale;

        //now the samplepoint has been more distribute,however, more points close the shader point
        samples.push_back(samplePoint);
    }
    {
        ssaoShader.use();
        ssaoShader.SetValue("ScreenWidth", (float) SCREENWIDTH);
        ssaoShader.SetValue("ScreenHeight", (float) SCREENHEIGHT);
        ssaoShader.SetValue("kernelSize", kernelSize);
        ssaoShader.SetValue("radius", radius);
        ssaoShader.SetValue("bias", 0.025f);
        for(int i = 0; i < kernelSize; i++)
        {
            ssaoShader.SetValue("samples[" + std::to_string(i) + "]", samples[i]);
        }
    }

    glm::vec3 lightPos = glm::vec3(2.0, 4.0, -2.0);
    glm::vec3 lightColor = glm::vec3(0.2, 0.2, 0.7);

    //blur ssao
    uint32_t ssaoBlur, ssaoBlurTex;
    {
        glGenFramebuffers(1, &ssaoBlur);
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlur);
        glGenTextures(1, &ssaoBlurTex);
        glBindTexture(GL_TEXTURE_2D, ssaoBlurTex);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCREENWIDTH, SCREENHEIGHT, 0, GL_RED, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoBlurTex, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    while (!glfwWindowShouldClose(window))
    {
        float nowTime = static_cast<float>(glfwGetTime());
        ProcessInput(window, nowTime - lastTime);
        lastTime = nowTime;

        glm::mat4 Matrix[2]{
            camera.GetViewMatrix(),
            camera.GetPerspectMatrix((float)SCREENWIDTH / (float)SCREENHEIGHT)
        };

        glBindBuffer(GL_UNIFORM_BUFFER, ubo);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4) * 2, glm::value_ptr(Matrix[0]));

        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //renderModel
        modelShader.use();
        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
        model = glm::scale(model, glm::vec3(1.0f));
        modelShader.SetValue("model", model);
        backobj.Draw(modelShader);

        //renderRoom
        roomShader.use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0, 7.0f, 0.0f));
        model = glm::scale(model, glm::vec3(7.5f, 7.5f, 7.5f));
        roomShader.SetValue("model", model);
        roomShader.SetValue("inverseNormal", true);
        roomShader.SetValue("color", glm::vec3(0.95f));
        renderCube();

        //calculate ssao value
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ssaoShader.use();
        ssaoShader.SetValue("gPosition", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        ssaoShader.SetValue("gNormal", 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        ssaoShader.SetValue("texNoise", 2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, texNoiseTex);
        ssaoShader.SetValue("view", camera.GetViewMatrix());
        ssaoShader.SetValue("perspective", camera.GetPerspectMatrix((float) SCREENWIDTH / (float) SCREENHEIGHT));
        renderQuad();

        //ssaoBlur
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlur);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_ssaoBlurShader.use();
        m_ssaoBlurShader.SetValue("ssaoTexture", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ssaoTex);
        renderQuad();
        
        //render scene
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_totalShader.use();
        m_totalShader.SetValue("gPosition", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        m_totalShader.SetValue("gNormal", 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        m_totalShader.SetValue("gAlbedoSpec", 2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gColor);
        m_totalShader.SetValue("ssaoRate", 3);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, ssaoBlurTex);
        m_totalShader.SetValue("ssaoDebug", 4);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, ssaoDebugTex);
        glm::vec4 viewPos = camera.GetViewMatrix() * glm::vec4(lightPos, 1.0f);
        glm::vec3 v = glm::vec3(viewPos / viewPos.w);
        m_totalShader.SetValue("light.Position", v);
        m_totalShader.SetValue("light.Color", lightColor);
        
        const float linear    = 0.09f;
        const float quadratic = 0.032f;
        m_totalShader.SetValue("light.linear", linear);
        m_totalShader.SetValue("light.quadratic", quadratic);
        m_totalShader.SetValue("uCameraPos", glm::vec3(0.0f,0.0f,0.0f));
        renderQuad();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
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

void renderCube()
{
    static VertexArray cubeVertex;
    static int count = 0;
    RenderData(cubeVertex, count, (int)RENDERTYPE::CUBE);
    cubeVertex.Bind();
    glDrawArrays(GL_TRIANGLES, 0, count);
}
void RenderScene(Shader& shaderProgram, const uint32_t& secondTex)
{
    shaderProgram.use();
    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0));
    model = glm::scale(model, glm::vec3(12.5f, 0.5f, 12.5f));
    shaderProgram.SetValue("model", model);
    renderCube();

    glBindTexture(GL_TEXTURE_2D, secondTex);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
    model = glm::scale(model, glm::vec3(0.5f));
    shaderProgram.SetValue("model", model);
    renderCube();

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
    model = glm::scale(model, glm::vec3(0.5f));
    shaderProgram.SetValue("model", model);
    renderCube();

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 2.0));
    model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
    shaderProgram.SetValue("model", model);
    renderCube();

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 2.7f, 4.0));
    model = glm::rotate(model, glm::radians(23.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
    model = glm::scale(model, glm::vec3(1.25));
    shaderProgram.SetValue("model", model);
    renderCube();

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-2.0f, 1.0f, -3.0));
    model = glm::rotate(model, glm::radians(124.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
    shaderProgram.SetValue("model", model);
    renderCube();

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-3.0f, 0.0f, 0.0));
    model = glm::scale(model, glm::vec3(0.5f));
    shaderProgram.SetValue("model", model);
    renderCube();
}


