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
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define WINDOWWIDTH 800.0f
#define WINDOWHEIGHT 600.0f
Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));
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
	
	float cubeVertices[] = {
		// Back face
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right         
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
		// Front face
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
		// Left face
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
		// Right face
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right         
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left     
		// Bottom face
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
		// Top face
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right     
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f  // bottom-left        
	};
	float planeVertices[] = {
		// positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
		5.0f, -0.5f, -5.0f,  2.0f, 2.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
		5.0f, -0.5f,  5.0f,  2.0f, 0.0f,

		5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
		-5.0f, -0.5f,  5.0f,  0.0f, 0.0f
	};
	float transparentVertices[] = {
		// positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
		0.0f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.0f, -0.5f,  0.0f,  0.0f,  0.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  0.0f,

		0.0f,  0.5f,  0.0f,  0.0f,  1.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  0.0f,
		1.0f,  0.5f,  0.0f,  1.0f,  1.0f
	};
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, -1.0f, 1.0f, 1.0f,
		 1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
		 1.0f, -1.0f, -1.0f, 1.0f, 1.0f,
		 1.0f,  1.0f, -1.0f, 0.0f, 0.0f,
		-1.0f,  1.0f, -1.0f, 0.0f, 1.0f,
							 
		-1.0f, -1.0f,  1.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
		-1.0f,  1.0f, -1.0f, 1.0f, 1.0f,
		-1.0f,  1.0f, -1.0f, 1.0f, 1.0f,
		-1.0f,  1.0f,  1.0f, 0.0f, 1.0f,
		-1.0f, -1.0f,  1.0f, 0.0f, 0.0f,
							 
		 1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 1.0f, 1.0f,
		 1.0f,  1.0f,  1.0f, 0.0f, 1.0f,
		 1.0f,  1.0f,  1.0f, 0.0f, 1.0f,
		 1.0f,  1.0f, -1.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
							 
		-1.0f, -1.0f,  1.0f, 1.0f, 0.0f,
		-1.0f,  1.0f,  1.0f, 0.0f, 1.0f,
		 1.0f,  1.0f,  1.0f, 1.0f, 1.0f,
		 1.0f,  1.0f,  1.0f, 0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f,  1.0f, 0.0f, 0.0f,
							 
		-1.0f,  1.0f, -1.0f, 0.0f, 1.0f,
		 1.0f,  1.0f, -1.0f, 1.0f, 1.0f,
		 1.0f,  1.0f,  1.0f, 1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f, 0.0f,
		-1.0f,  1.0f,  1.0f, 0.0f, 0.0f,
		-1.0f,  1.0f, -1.0f, 0.0f, 1.0f,
							 
		-1.0f, -1.0f, -1.0f, 0.0f, 1.0f,
		-1.0f, -1.0f,  1.0f, 1.0f, 0.0f,
		 1.0f, -1.0f, -1.0f, 1.0f, 1.0f,
		 1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f,  1.0f, 0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f, 0.0f
	};
	float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};
	OpenGLVertexArray cubeVertexArray(cubeVertices,sizeof(cubeVertices),1);
	OpenGLVertexArray planeVertexArray(planeVertices,sizeof(planeVertices),1);
	OpenGLVertexArray transparentVetexArray(transparentVertices, sizeof(transparentVertices),1);
	OpenGLVertexArray skyVertexArray(skyboxVertices, sizeof(skyboxVertices),1);
	OpenGLVertexArray useEnvirment(vertices, sizeof(vertices),2);
	Texture cubeTexture("D:/learnOpengl/LearnOpenGL/DeeperOpenGL/DeeperOpenGL/assets/texture/marble.jpg");
	Texture planeTexture("D:/learnOpengl/LearnOpenGL/DeeperOpenGL/DeeperOpenGL/assets/texture/metal.png");
	Texture grassTexture("D:/learnOpengl/LearnOpenGL/DeeperOpenGL/DeeperOpenGL/assets/texture/grass.png");
	Texture glassTexture("D:/learnOpengl/LearnOpenGL/DeeperOpenGL/DeeperOpenGL/assets/texture/glass.png");
	Texture boxTexture  ("D:/learnOpengl/LearnOpenGL/DeeperOpenGL/DeeperOpenGL/assets/texture/container.jpg");
	std::string sky_base_path = "D:/learnOpengl/LearnOpenGL/DeeperOpenGL/DeeperOpenGL/assets/texture/skybox/";
	std::vector<std::string> skyPic{
		sky_base_path + "right.jpg",
		sky_base_path + "left.jpg",
		sky_base_path + "top.jpg",
		sky_base_path + "bottom.jpg",
		sky_base_path + "front.jpg",
		sky_base_path + "back.jpg"
	};
	CubeTexture sky(skyPic);
	Shader m_TextureShader("D:/learnOpengl/LearnOpenGL/DeeperOpenGL/DeeperOpenGL/src/shader/depth_test.vs", "D:/learnOpengl/LearnOpenGL/DeeperOpenGL/DeeperOpenGL/src/shader/depth_test.fs");
	Shader m_boxShader("D:/learnOpengl/LearnOpenGL/DeeperOpenGL/DeeperOpenGL/src/shader/frambuffer.vs", "D:/learnOpengl/LearnOpenGL/DeeperOpenGL/DeeperOpenGL/src/shader/frambuffer.fs");
	Shader m_skyShader("D:/learnOpengl/LearnOpenGL/DeeperOpenGL/DeeperOpenGL/src/shader/cube_map.vs", "D:/learnOpengl/LearnOpenGL/DeeperOpenGL/DeeperOpenGL/src/shader/cube_map.fs");
	m_TextureShader.use();
	m_TextureShader.setInt("u_Texture", 0);
	m_skyShader.use();
	m_skyShader.setInt("sky", 0);
	float lastTime = static_cast<float>(glfwGetTime());
	

	
	while (!glfwWindowShouldClose(window)) {
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float currentFram = static_cast<float> (glfwGetTime());
		float delteTime = currentFram - lastTime;
		lastTime = currentFram;
		KeycallBack(window, delteTime);

		//sky cube
		glDepthFunc(GL_LEQUAL);
		glm::mat4  model2 = glm::mat4(1.0f);
		//model2 = glm::translate(model2, glm::vec3(2.0f, 0.0f, 0.0f));
		Render::BeginScene(m_skyShader, skyVertexArray);
		glm::mat4 view = camera.getViewMatrix();
		view = glm::mat4(glm::mat3(view));
		glm::mat4 perspective = camera.getPerspective(WINDOWWIDTH, WINDOWHEIGHT);
		m_skyShader.setMat4("view", view);
		m_skyShader.setMat4("perspective", perspective);
		m_skyShader.setMat4("model", model2);
		sky.Bind();
		Render::RenderScene(36);

		//plane
		/*Render::BeginScene(m_TextureShader, planeVertexArray);
		m_TextureShader.setMat4("model", glm::mat4(1.0f));
		planeTexture.Bind();
		Render::RenderScene(6);*/

		//cube
		glDepthFunc(GL_LESS);
		Render::BeginScene(m_TextureShader, useEnvirment);
		glm::mat4 model1 = glm::mat4(1.0f);
		model1 = glm::translate(model1, glm::vec3(-1.0f, 0.0f, -1.0f));
		m_TextureShader.setMat4("model", model1);
		glm::mat4 perview = camera.getViewPerspective(WINDOWWIDTH, WINDOWHEIGHT);
		m_TextureShader.setMat4("projection", perview);
		m_TextureShader.setVec3("cameraPos", camera.GetPosition());
		sky.Bind();
		Render::RenderScene(36);

		
		

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
