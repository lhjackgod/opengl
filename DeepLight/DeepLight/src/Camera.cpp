#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
const float Camera::MuseSentity = 1.0f;
const float Camera::speed = 1.0f;
Camera::Camera(glm::vec3 position, float aspectRatio)
	:m_Position(position), m_AspectRatio(aspectRatio)
{
	Recalculate();
}

void Camera::MouseMove(float xoffset, float yoffset, bool isConstantPitch)
{
	xoffset *= MuseSentity;
	yoffset *= MuseSentity;

	pitch += yoffset;
	if (isConstantPitch)
	{
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
	}
	yaw += xoffset;
	Recalculate();
}

void Camera::KeyClick(KEYTYPE type, float deltaTime)
{
	float offset = deltaTime * speed;
	switch (type)
	{
	case KEYTYPE::FORWARD:
		m_Position += offset * m_Front;
		break;
	case KEYTYPE::BACK:
		m_Position -= offset * m_Front;
		break;
	case KEYTYPE::LEFT:
		m_Position -= offset * m_Right;
		break;
	case KEYTYPE::RIGHT:
		m_Position += offset * m_Right;
		break;
	}
}

void Camera::MouseScroll(float yoffset)
{
	std::cout << yoffset << std::endl;
	m_FOV -= yoffset;
	if (m_FOV >= 45.0f)
		m_FOV = 45.0f;
	else if (m_FOV < 1.0f)
		m_FOV = 1.0f;
	std::cout << m_FOV << std::endl;
}

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(m_Position, m_Front + m_Position, m_Up);
}

glm::mat4 Camera::GetPerspectMatrix(const float& aspect)
{
	return glm::perspective(m_FOV, aspect, 0.1f, 100.0f);
}

void Camera::Recalculate()
{
	glm::vec3& front = m_Front;
	front.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
	front.y = glm::sin(glm::radians(pitch));
	front.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
	front = glm::normalize(front);

	m_Right = glm::normalize(glm::cross(front, WorldUP));
	m_Up = glm::normalize(glm::cross(m_Right, m_Front));
}
