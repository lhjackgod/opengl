#include "Camera.h"
#include <iostream>
void Camera::ProcessKeyCallback(CameraMovement movement, float deltaTime)
{
	
	float positionOffset = deltaTime * CameraMovementSpeed;
	if (movement == CameraMovement::FORWARD) {
		position += front * positionOffset;
	}
	if (movement == CameraMovement::BACKWARD) {
		position -= front * positionOffset;
	}
	if (movement == CameraMovement::LEFT) {
		position -= right * positionOffset;
	}
	if (movement == CameraMovement::RIGHT) {
		position += right * positionOffset;
	}
}

void Camera::ProcessMouseCallback(float xoffset, float yoffset, GLboolean constrainPitch)
{
	xoffset *= MouseSensitive;
	yoffset *= MouseSensitive;
	yaw += xoffset;
	patch += yoffset;
	if (constrainPitch) {
		if (patch < -89.0f) patch = -89.0f;
		else if (patch > 89.0f) patch = 89.0f;
	}
	
	updateInfo();
}

void Camera::ProcessScrollCallback(float yoffset)
{
	fov += yoffset;
	if (fov < 1.0f) fov = 1.0f;
	else if (fov > 90.0f) fov = 90.0f;
}

float Camera::getFov()
{
	return fov;
}

glm::mat4 Camera::getViewMatrix()
{
	return glm::lookAt(position, front + position, up);
}

void Camera::updateInfo()
{
	glm::vec3 Front;
	Front.x = static_cast<float>(std::cos(yaw) * std::cos(patch));
	Front.y = static_cast<float>(std::sin(patch));
	Front.z = static_cast<float>(std::sin(yaw) * std::cos(patch));
	Front = glm::normalize(Front);
	front = Front;
	right = glm::cross(front, WORLD_UP);
	up = glm::cross(right, front);
}
