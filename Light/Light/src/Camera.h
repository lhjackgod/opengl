#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
enum  class CameraMovement
{
	FORWARD, BACKWARD, LEFT, RIGHT
};

class Camera
{
public:
	Camera(glm::vec3 position = glm::vec3(0.0f,0.0f,0.0f), glm::vec3 front = glm::vec3(0.0f,0.0f,-1.0f),
		float yaw = -90.0f, float patch = 0.0f):
		position(position), front(front), yaw(yaw), patch(patch) {
		updateInfo();
	}
	void ProcessKeyCallback(CameraMovement movement, float deltaTime);
	void ProcessMouseCallback(float xoffset, float yoffset, GLboolean constrainPitch = true);
	void ProcessScrollCallback(float yoffset);
	float getFov();
	glm::vec3 getCameraPos();
	glm::mat4 getViewMatrix();
	glm::vec3 getFront();
private:
	float fov = 45.0f;
	glm::vec3 position;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 front;
	glm::vec3 WORLD_UP = glm::vec3(0.0f, 1.0f, 0.0f);
	float yaw;
	float patch;
	float CameraMovementSpeed = 2.5f;
	float MouseSensitive = 0.005f;
private:
	void updateInfo();
};

