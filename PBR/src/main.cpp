#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include "Camera.h"
#include "Shader.h"
#include "stb_image.h"
#define SCREENWIDTH 800
#define SCREENHEIGHT 600

float deltaTime = 0.0f;
bool firstMove = true;
float lastX = SCREENWIDTH / 2.0f;
float lastY = SCREENHEIGHT / 2.0f;
Camera mainCamera(glm::vec3(1.2554f, 4.59254f, 7.34487f));
void processInputKey(GLFWwindow* window, int key, int scancode, int action, int mods);
void processRote(GLFWwindow* window,double pitchOffset, double yawOffset);
void processFOV(GLFWwindow* window, double xoffset, double yoffset);
void renderSphere();
uint32_t getImage(const std::string& imagePath);
int main()
{
	glfwInit();
	glfwWindowHint(GLFW_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_CORE_PROFILE, GLFW_OPENGL_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCREENWIDTH, SCREENHEIGHT, "pbr", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "error create glad" << std::endl;
		return EXIT_FAILURE;
	}
	glViewport(0, 0, SCREENWIDTH, SCREENHEIGHT);
	glfwSetKeyCallback(window, processInputKey);
	glfwSetCursorPosCallback(window, processRote);
	
	glfwSetScrollCallback(window, processFOV);

	Shader phereShader("src/shader/pbr/Sphere.vert", "src/shader/pbr/Sphere.frag");
	Shader cubeShader("src/shader/ibl/cube.vert", "src/shader/ibl/cube.frag");

	float lastTime = static_cast<float>(glfwGetTime());

	uint32_t uniformBuffer;
	glGenBuffers(1, &uniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, uniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, 2, uniformBuffer, 0, 2 * sizeof(glm::mat4));


	//lights
	glm::vec3 lightPositions[] = {
		glm::vec3(-10.0f,  10.0f, 10.0f),
		glm::vec3(10.0f,  10.0f, 10.0f),
		glm::vec3(-10.0f, -10.0f, 10.0f),
		glm::vec3(10.0f, -10.0f, 10.0f),
	};
	glm::vec3 lightColors[] = {
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f)
	};

	int nrRows = 7;
	int nrColums = 7;
	float spacing = 2.5f;
	while (!glfwWindowShouldClose(window))
	{
		float currentTime = static_cast<float>(glfwGetTime());
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;
		
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glm::mat4 view = mainCamera.getViewMatrix();
		glm::mat4 perspective = mainCamera.getPerspective(static_cast<float>(SCREENWIDTH / SCREENHEIGHT));
		glBindBuffer(GL_UNIFORM_BUFFER, uniformBuffer);
		glm::mat4 PMatrix[2]{ view, perspective };
		glBufferSubData(GL_UNIFORM_BUFFER, 0, 2 * sizeof(glm::mat4), glm::value_ptr(PMatrix[0]));
		phereShader.use();
		glm::mat4 model(1.0f);
		phereShader.setValue("cameraPos", mainCamera.getPosition());
		phereShader.setValue("workAlbedo", glm::vec3(0.5f, 0.0f, 0.0f));
		for (int i = 0; i < 4; i++)
		{
			phereShader.setValue("lightMessage[" + std::to_string(i) + "].position", lightPositions[i]);
			phereShader.setValue("lightMessage[" + std::to_string(i) + "].color", lightColors[i]);
		}
		
		for (int row = 0; row < nrRows; ++row)
		{
			phereShader.setValue("metallic", (float)row / (float)nrRows);
			for (int col = 0; col < nrColums; ++col)
			{
				// we clamp the roughness to 0.05 - 1.0 as perfectly smooth surfaces (roughness of 0.0) tend to look a bit off
				// on direct lighting.
				phereShader.setValue("roughness", glm::clamp((float)col / (float)nrColums, 0.05f, 1.0f)); 

				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(
					(col - (nrColums / 2)) * spacing,
					(row - (nrRows / 2)) * spacing,
					0.0f
				));
				phereShader.setValue("model", model);
				phereShader.setValue("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
				renderSphere();
			}
		}
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
}

void processInputKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT))
	{
		mainCamera.InputProcessKey(INPUTKEY::KEY_A, deltaTime);
	}
	else if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT))
	{
		mainCamera.InputProcessKey(INPUTKEY::KEY_D, deltaTime);
	}
	else if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT))
	{
		mainCamera.InputProcessKey(INPUTKEY::KEY_S, deltaTime);
	}
	else if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT))
	{
		mainCamera.InputProcessKey(INPUTKEY::KEY_W, deltaTime);
	}
}

void processRote(GLFWwindow* window, double yawOffset, double pitchOffset)
{
	float xPos = static_cast<float>(yawOffset);
	float yPos = static_cast<float>(pitchOffset);
	if (firstMove)
	{
		lastX = xPos;
		lastY = yPos;
		firstMove = false;
	}
	float xoffset = xPos - lastX;
	float yoffset = lastY - yPos;
	lastX = xPos;
	lastY = yPos;
	mainCamera.InputProcessMouse(xoffset, yoffset);
	
}

void processFOV(GLFWwindow* window, double xoffset, double yoffset)
{
	mainCamera.InputProcessFov(static_cast<float>(yoffset));
}

unsigned int sphereVAO;
unsigned int indexCount;
void renderSphere()
{
	if (sphereVAO == 0)
	{
		glGenVertexArrays(1, &sphereVAO);
		unsigned int vbo, ebo;
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);
		
		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> uv;
		std::vector<glm::vec3> normals;
		std::vector<unsigned int> indices;

		const unsigned int X_SEGMENTS = 64;
		const unsigned int Y_SEGMENTS = 64;
		const float PI = 3.14159265359f;

		for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
		{
			for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
			{
				float xSegment = (float)x / (float)X_SEGMENTS;
				float ySegment = (float)y / (float)Y_SEGMENTS;
				float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
				float yPos = std::cos(ySegment * PI);
				float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

				positions.push_back(glm::vec3(xPos, yPos, zPos));
				uv.push_back(glm::vec2(xSegment, ySegment));
				normals.push_back(glm::vec3(xPos, yPos, zPos));
			}
		}
		bool oddRow = false;
		for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
		{
			if (!oddRow) // even rows: y == 0, y == 2; and so on
			{
				for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
				{
					indices.push_back(y * (X_SEGMENTS + 1) + x);
					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
				}
			}
			else
			{
				for (int x = X_SEGMENTS; x >= 0; --x)
				{
					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
					indices.push_back(y * (X_SEGMENTS + 1) + x);
				}
			}
			oddRow = !oddRow;
		}
		indexCount = static_cast<unsigned int>(indices.size());
		std::vector<float> data;
		for (int i = 0; i < positions.size(); i++)
		{
			data.push_back(positions[i].x);
			data.push_back(positions[i].y);
			data.push_back(positions[i].z);
			if (normals.size() > 0)
			{
				data.push_back(normals[i].x);
				data.push_back(normals[i].y);
				data.push_back(normals[i].z);
			}
			if (uv.size() > 0)
			{
				data.push_back(uv[i].x);
				data.push_back(uv[i].y);
			}
		}
		glBindVertexArray(sphereVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);
		glBindVertexArray(0);
	}
	glBindVertexArray(sphereVAO);
	glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}

uint32_t getImage(const std::string& imagePath)
{
	stbi_set_flip_vertically_on_load(true);
	int width, height;

}