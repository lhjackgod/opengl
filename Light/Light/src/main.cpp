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
	glm::vec3 cubePositions[] = {
	glm::vec3(0.0f,  0.0f,  0.0f),
	glm::vec3(2.0f,  5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f,  3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f,  2.0f, -2.5f),
	glm::vec3(1.5f,  0.2f, -1.5f),
	glm::vec3(-1.3f,  1.0f, -1.5f)
	};
	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};
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
		lightingShader.setVec3("cameraPos", camera.getCameraPos());
		lightingShader.setFloat("material.k", 64.0f);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);
		lightingShader.setVec3("pL.direction", -0.2f, -1.0f, -0.3f);
		lightingShader.setVec3("ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pL.specular", 0.5f, 0.5f, 0.5f);
		lightingShader.setVec3("pL.diffuse", 0.4f, 0.4f, 0.4f);

		lightingShader.setVec3("point_light[0].position", pointLightPositions[0]);
		lightingShader.setFloat("point_light[0].constant", 1.0f);
		lightingShader.setFloat("point_light[0].linear", 0.09f);
		lightingShader.setFloat("point_light[0].quadtratic", 0.032f);
		lightingShader.setVec3("point_light[0].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("point_light[0].specular", 1.0f, 1.0f, 1.0f);

		lightingShader.setVec3("point_light[1].position", pointLightPositions[1]);
		lightingShader.setFloat("point_light[1].constant", 1.0f);
		lightingShader.setFloat("point_light[1].linear", 0.09f);
		lightingShader.setFloat("point_light[1].quadtratic", 0.032f);
		lightingShader.setVec3("point_light[1].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("point_light[1].specular", 1.0f, 1.0f, 1.0f);

		lightingShader.setVec3("point_light[2].position", pointLightPositions[2]);
		lightingShader.setFloat("point_light[2].constant", 1.0f);
		lightingShader.setFloat("point_light[2].linear", 0.09f);
		lightingShader.setFloat("point_light[2].quadtratic", 0.032f);
		lightingShader.setVec3("point_light[2].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("point_light[2].specular", 1.0f, 1.0f, 1.0f);

		lightingShader.setVec3("point_light[3].position", pointLightPositions[3]);
		lightingShader.setFloat("point_light[3].constant", 1.0f);
		lightingShader.setFloat("point_light[3].linear", 0.09f);
		lightingShader.setFloat("point_light[3].quadtratic", 0.032f);
		lightingShader.setVec3("point_light[3].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("point_light[3].specular", 1.0f, 1.0f, 1.0f);

		lightingShader.setVec3("flashlight.position", camera.getCameraPos());
		lightingShader.setVec3("flashlight.diffuse", 1.0f, 1.0f, 1.0f);
		lightingShader.setVec3("flashlight.specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setVec3("flashlight.direction", camera.getFront());
		lightingShader.setFloat("flashlight.outCutOff", glm::cos(glm::radians(17.5f)));
		lightingShader.setFloat("flashlight.cutOff", glm::cos(glm::radians(12.5f)));
		lightingShader.setFloat("flashlight.constant", 1.0f);
		lightingShader.setFloat("flashlight.linear", 0.09f);
		lightingShader.setFloat("flashlight.quadtratic", 0.032f);
		for (int i = 0; i < 10; i++) {
			model = glm::mat4(1.0);
			model = glm::translate(model, cubePositions[i]);
			float angle = i * 20.0f;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			lightingShader.setMatrix4("model", model);
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		lightcube.use();
		lightcube.setMatrix4("view", view);
		lightcube.setMatrix4("perspective", proj);
		for (int i = 0; i < 4; i++) {
			modellight = glm::mat4(1.0);
			modellight = glm::translate(modellight, pointLightPositions[i]);
			modellight = glm::scale(modellight, glm::vec3(0.2f));
			lightcube.setMatrix4("model", modellight);
			glBindVertexArray(VAOLight);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}