#include "Camera.h"
const glm::vec3 Camera::WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
const float Camera::m_KeyMoveSpeed = 2.5f;
const float Camera::m_MouseSensitive = 0.01f;

Camera::Camera(glm::vec3 position):
	m_Position(position)
{
	reCalculate();
}

void Camera::KeyImpl(const KEYTYPE& keytype, const float& deltatime)
{
	float offset = deltatime * m_KeyMoveSpeed;
	if (keytype == KEYTYPE::LH_KEY_A) {
		m_Position -= m_Right * offset;
	}
	else if (keytype == KEYTYPE::LH_KEY_D) {
		m_Position += m_Right * offset;
	}
	else if (keytype == KEYTYPE::LH_KEY_S) {
		m_Position -= m_Front * offset;
	}
	else if (keytype == KEYTYPE::LH_KEY_W) {
		m_Position += m_Front * offset;
	}
}

void Camera::MouseImpl(float xoffset, float yoffset, GLboolean constraintPitch)
{
	xoffset *= m_MouseSensitive;
	yoffset *= m_MouseSensitive;
	m_Yaw += xoffset;
	m_Pitch += yoffset;
	if (constraintPitch) {
		m_Pitch = m_Pitch < -89.0f ? -89.0f : m_Pitch;
		m_Pitch = m_Pitch > 89.0f ? 89.0f : m_Pitch;
	}
	reCalculate();
}

void Camera::ZoomImpl(const float& yoffset)
{
	m_Zoom -= yoffset;
	if (m_Zoom < 1.0f) m_Zoom = 1.0f;
	else if (m_Zoom > 45.0f) m_Zoom = 45.0f;
}

glm::mat4 Camera::getViewMatrix()
{
	return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
}

glm::mat4 Camera::getViewPerspective(const float& width, const float& height)
{
	return glm::perspective(glm::radians(m_Zoom), width / height, 0.1f, 100.0f) *
		glm::lookAt(m_Position, m_Front + m_Position, m_Up);
}

glm::mat4 Camera::getPerspective(const float& width, const float& height)
{
	return glm::perspective(glm::radians(m_Zoom), width / height, 0.1f, 100.0f);
}

glm::vec3 Camera::GetPosition()
{
	return m_Position;
}

void Camera::SetYaw(float offset)
{
	m_Yaw += offset;
	reCalculate();
}

void Camera::reCalculate()
{
	glm::vec3 front;
	front.x = glm::cos(m_Yaw) * glm::cos(m_Pitch);
	front.y = glm::sin(m_Pitch);
	front.z = glm::sin(m_Yaw) * glm::cos(m_Pitch);

	m_Front = glm::normalize(front);
	m_Right = glm::normalize(glm::cross(m_Front, WorldUp));
	m_Up = glm::normalize(glm::cross(m_Right, m_Front));
}
