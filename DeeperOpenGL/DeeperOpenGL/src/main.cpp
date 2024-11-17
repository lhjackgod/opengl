#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <map>
#include <fstream>
#include <sstream>
#include "Camera.h"
#include "Shader.h"
#include "Render.h"
#include "Texture.h"
#include "OpenGLVertexArray.h"
#include "Model.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define WINDOWWIDTH 800.0f
#define WINDOWHEIGHT 600.0f
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = (float)WINDOWWIDTH / 2.0f;
float lastY = (float)WINDOWHEIGHT / 2.0f;
bool firtstMouse = true;
void KeycallBack(GLFWwindow* window, float deltetime);
void MouseCallBack(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_OPENGL_CORE_PROFILE, GLFW_OPENGL_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WINDOWWIDTH, WINDOWHEIGHT, "deeperOpengl", NULL, NULL);
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		return -1;
	}
	
	glfwSetCursorPosCallback(window, MouseCallBack);
	glfwSetWindowSizeCallback(window, framebuffer_size_callback);
	glfwSetScrollCallback(window, scroll_callback);

	GLfloat cubeVertices[] = {
        // Positions       
        -0.5f, -0.5f, -0.5f, 
         0.5f, -0.5f, -0.5f, 
         0.5f,  0.5f, -0.5f, 
         0.5f,  0.5f, -0.5f,  
        -0.5f,  0.5f, -0.5f,  
        -0.5f, -0.5f, -0.5f, 

        -0.5f, -0.5f,  0.5f, 
         0.5f, -0.5f,  0.5f, 
         0.5f,  0.5f,  0.5f, 
         0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f,  0.5f, 
        -0.5f, -0.5f,  0.5f, 

        -0.5f,  0.5f,  0.5f, 
        -0.5f,  0.5f, -0.5f,  
        -0.5f, -0.5f, -0.5f,  
        -0.5f, -0.5f, -0.5f, 
        -0.5f, -0.5f,  0.5f, 
        -0.5f,  0.5f,  0.5f, 

         0.5f,  0.5f,  0.5f,  
         0.5f,  0.5f, -0.5f, 
         0.5f, -0.5f, -0.5f, 
         0.5f, -0.5f, -0.5f, 
         0.5f, -0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  

        -0.5f, -0.5f, -0.5f, 
         0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f,  0.5f, 
         0.5f, -0.5f,  0.5f, 
        -0.5f, -0.5f,  0.5f,  
        -0.5f, -0.5f, -0.5f,  

        -0.5f,  0.5f, -0.5f,  
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f, -0.5f
    };
	float quadVertices[] = {   // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,

        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f
    };
	OpenGLVertexArray m_CubeArray(cubeVertices, sizeof(cubeVertices), 0);
	OpenGLVertexArray m_quadArray(quadVertices, sizeof(quadVertices), 1);
	Shader m_cubeShader("D:/learnOpengl/LearnOpenGL/DeeperOpenGL/DeeperOpenGL/src/shader/cube.vs","D:/learnOpengl/LearnOpenGL/DeeperOpenGL/DeeperOpenGL/src/shader/cube.fs");
	Shader m_quadShader("D:/learnOpengl/LearnOpenGL/DeeperOpenGL/DeeperOpenGL/src/shader/quad.vs","D:/learnOpengl/LearnOpenGL/DeeperOpenGL/DeeperOpenGL/src/shader/quad.fs");
	
	float lastTime = static_cast<float>(glfwGetTime());
	
	unsigned int ubo;
	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo, 0, 2 * sizeof(glm::mat4));
	
	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	unsigned int colormb;
	glGenTextures(1, &colormb);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, colormb);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, WINDOWWIDTH, WINDOWHEIGHT, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, colormb, 0);

	unsigned int renderbufferm;
	glGenRenderbuffers(1, &renderbufferm);
	glBindRenderbuffer(GL_RENDERBUFFER ,renderbufferm);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, WINDOWWIDTH, WINDOWHEIGHT);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbufferm);
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER)!=GL_FRAMEBUFFER_COMPLETE){
		std::cout<<"error"<<std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	unsigned int frambuffer;
	glGenFramebuffers(1, &frambuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frambuffer);

	unsigned int sceenTex;
	glGenTextures(1, &sceenTex);
	glBindTexture(GL_TEXTURE_2D, sceenTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOWWIDTH, WINDOWHEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sceenTex, 0);
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER)!=GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout<<"error2"<<std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	m_quadShader.use();
	m_quadShader.setInt("tex", 0);
	while (!glfwWindowShouldClose(window)) {
		
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		float currentFram = static_cast<float> (glfwGetTime());
		float delteTime = currentFram - lastTime;
		lastTime = currentFram;
		KeycallBack(window, delteTime);
		glm::mat4 view = camera.getViewMatrix();
		glm::mat4 perspective = camera.getPerspective(WINDOWWIDTH, WINDOWHEIGHT);
		glm::mat4 uboMatrices[2];
		uboMatrices[0] = view;
		uboMatrices[1] = perspective;
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(uboMatrices), glm::value_ptr(uboMatrices[0]));
		
		Render::BeginScene(m_cubeShader, m_CubeArray);
		m_cubeShader.setMat4("model", glm::mat4(1.0f));
		Render::RenderScene(36);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frambuffer);
		glBlitFramebuffer(0, 0, WINDOWWIDTH, WINDOWHEIGHT, 0, 0, WINDOWWIDTH, WINDOWHEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.0,0.0,0.0,1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);

		Render::BeginScene(m_quadShader, m_quadArray);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, sceenTex);
		Render::RenderScene(6);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
}

void KeycallBack(GLFWwindow* window, float deltatime)
{
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.KeyImpl(KEYTYPE::LH_KEY_A, deltatime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.KeyImpl(KEYTYPE::LH_KEY_D, deltatime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.KeyImpl(KEYTYPE::LH_KEY_S, deltatime);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.KeyImpl(KEYTYPE::LH_KEY_W, deltatime);
}

void MouseCallBack(GLFWwindow* window, double xpos, double ypos)
{
	float x_pos = static_cast<float>(xpos);
	float y_pos = static_cast<float>(ypos);

	if (firtstMouse) {
		lastX = x_pos;
		lastY = y_pos;
		firtstMouse = false;
	}
	float xoffset = x_pos - lastX;
	float yoffset = lastY - y_pos;
	lastX = x_pos;
	lastY = y_pos;
	camera.MouseImpl(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ZoomImpl(static_cast<float>(yoffset));
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
