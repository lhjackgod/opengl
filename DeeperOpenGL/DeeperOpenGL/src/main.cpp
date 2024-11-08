#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
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
	
	float cubeVertices[] = {
		// positions          // texture Coords
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};
	float planeVertices[] = {
		// positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
		 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

		 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
		 5.0f, -0.5f, -5.0f,  2.0f, 2.0f
	};
	OpenGLVertexArray cubeVertexArray(cubeVertices,sizeof(cubeVertices));
	OpenGLVertexArray planeVertexArray(planeVertices,sizeof(planeVertices));

	Texture cubeTexture("D:/learnOpengl/LearnOpenGL/DeeperOpenGL/DeeperOpenGL/assets/texture/marble.jpg");
	Texture planeTexture("D:/learnOpengl/LearnOpenGL/DeeperOpenGL/DeeperOpenGL/assets/texture/metal.png");
	Shader m_TextureShader("D:/learnOpengl/LearnOpenGL/DeeperOpenGL/DeeperOpenGL/src/shader/depth_test.vs", "D:/learnOpengl/LearnOpenGL/DeeperOpenGL/DeeperOpenGL/src/shader/depth_test.fs");
	m_TextureShader.use();
	m_TextureShader.setInt("u_Texture", 0);
	float lastTime = static_cast<float>(glfwGetTime());
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	while (!glfwWindowShouldClose(window)) {
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glStencilMask(0xFF);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		float currentFram = static_cast<float> (glfwGetTime());
		float delteTime = currentFram - lastTime;
		lastTime = currentFram;
		KeycallBack(window, delteTime);

		m_TextureShader.use();
		m_TextureShader.setVec4("u_Color", glm::vec4(0.0f));
		glm::mat4 perView = camera.getViewPerspective(static_cast<float>(WINDOWWIDTH), static_cast<float>(WINDOWHEIGHT));
		m_TextureShader.setMat4("projection", perView);
		

		//plane
		glStencilFunc(GL_ALWAYS, 0, 0xFF);
		glStencilMask(0xFF);
		Render::BeginScene(m_TextureShader, planeVertexArray);
		m_TextureShader.setMat4("model", glm::mat4(1.0f));
		planeTexture.Bind();
		Render::RenderScene(6);

		//cube
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
		Render::BeginScene(m_TextureShader, cubeVertexArray);
		glm::mat4 model1 = glm::mat4(1.0f);
		model1 = glm::translate(model1, glm::vec3(-1.0f, 0.0f, -1.0f));
		m_TextureShader.setMat4("model", model1);
		cubeTexture.Bind();
		Render::RenderScene(36);

		glm::mat4  model2 = glm::mat4(1.0f);
		model2 = glm::translate(model2, glm::vec3(2.0f, 0.0f, 0.0f));
		m_TextureShader.setMat4("model", model2);
		Render::RenderScene(36);

		//borader
		model1 = glm::scale(model1, glm::vec3(1.1f));
		m_TextureShader.setMat4("model", model1);
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		glDisable(GL_DEPTH_TEST);
		m_TextureShader.setVec4("u_Color", glm::vec4(1.0f));
		Render::RenderScene(36);

		model2 = glm::scale(model2, glm::vec3(1.1f));
		m_TextureShader.setMat4("model", model2);
		Render::RenderScene(36);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
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
