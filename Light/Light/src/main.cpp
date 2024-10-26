#include<glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include "Shader.h"
#include "Camera.h"

#define WINDOWWIDTH 800
#define WINDOWHEIGHT 600

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool firstMouseMove = true;
float lastXPos, lastYPos;
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
void ProcessInput(GLFWwindow* window, float deltaTime) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyCallback(CameraMovement::FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyCallback(CameraMovement::BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyCallback(CameraMovement::LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyCallback(CameraMovement::RIGHT, deltaTime);
}
void MouseMoveCallback(GLFWwindow* window, double Xpos, double Ypos) {
	float xpos = static_cast<float>(Xpos);
	float ypos = static_cast<float>(Ypos);
	if (firstMouseMove) {
		lastXPos = xpos;
		lastYPos = ypos;
		firstMouseMove = false;
	}
	float xoffset = xpos - lastXPos;
	float yoffset = lastYPos - ypos;
	lastXPos = xpos;
	lastYPos = ypos;
	camera.ProcessMouseCallback(xoffset, yoffset);
}
void MouseScrollCallback(GLFWwindow* window, double xO, double yO) {
	camera.ProcessScrollCallback(static_cast<float>(yO));
}
unsigned int setMaterial(const char* path) {
	unsigned int Texture;
	glGenTextures(1, &Texture);
	int width, height, channel;
	unsigned char* data = stbi_load(path, &width, &height, &channel, 0);
	GLenum format;
	if (data) {
		if (channel == 1) {
			format = GL_RED;
		}
		else if (channel == 3) {
			format = GL_RGB;
		}
		else if (channel == 4) {
			format = GL_RGBA;
		}
		glBindTexture(GL_TEXTURE_2D, Texture);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		std::cout << "error" << std::endl;
	}
	stbi_image_free(data);
	return Texture;
}
int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(WINDOWWIDTH, WINDOWHEIGHT, "Light", NULL, NULL);
	if (window == nullptr) { return -1; }
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		return -1;
	}
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glfwSetCursorPosCallback(window, MouseMoveCallback);
	glfwSetScrollCallback(window, MouseScrollCallback);
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {glViewport(0, 0, width, height); });
	Shader lightingShader("src/shader/renderCube.glsl", "src/shader/renderCubefrag.glsl");
	lightingShader.use();
	lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
	lightingShader.setInt("material.diffuse", 0);
	lightingShader.setInt("material.specular", 1);
	Shader lightcube("src/shader/vershader.glsl", "src/shader/fragment.glsl");
	lightcube.use();
	lightcube.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
	lightcube.setVec3("objColor", 1.0f, 1.0f, 1.0f);
	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};
	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 3));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 6));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	unsigned int VAOLight, VBOLight;
	glGenVertexArrays(1, &VAOLight);
	glGenBuffers(1, &VBOLight);
	glBindVertexArray(VAOLight);
	glBindBuffer(GL_ARRAY_BUFFER, VBOLight);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)0);
	glEnableVertexAttribArray(0);
	float lastTime = static_cast<float>(glfwGetTime());
	glm::mat4 model(1.0);
	glm::mat4 proj, view;
	glm::mat4 modellight(1.0f);
	modellight = glm::translate(modellight, lightPos);
	modellight = glm::scale(modellight, glm::vec3(0.2f));
	unsigned int texture1 = setMaterial("../Texture/container2.png");
	unsigned int texture2 = setMaterial("../Texture/container2_specular.png");
	while (!glfwWindowShouldClose(window)) {
		float nowTime = static_cast<float>(glfwGetTime());
		float deltaTime = nowTime - lastTime;
		lastTime = nowTime;
		ProcessInput(window,deltaTime);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		lightingShader.use();
		proj = glm::perspective(glm::radians(camera.getFov()), static_cast<float>(WINDOWWIDTH / WINDOWHEIGHT), 0.01f, 100.0f);
		view = camera.getViewMatrix();
		lightingShader.setMatrix4("view", view);
		lightingShader.setMatrix4("perspective", proj);
		lightingShader.setMatrix4("model", model);
		lightingShader.setVec3("light.position", lightPos);
		lightingShader.setVec3("cameraPos", camera.getCameraPos());
		lightingShader.setFloat("phonMaterial.k", 64.0f);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);
		lightingShader.setVec3("phonMaterial.ambientMaterial", glm::vec3(1.0f, 0.5f, 0.31f));
		glm::vec3 lightColor(1.0f);
		
		lightingShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
		lightingShader.setVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
		lightingShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		
		
		
		lightcube.use();
		lightcube.setMatrix4("view", view);
		lightcube.setMatrix4("perspective", proj);
		lightcube.setMatrix4("model", modellight);
		glBindVertexArray(VAOLight);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}