#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
enum class KEYTYPE
{
	FORWARD = 0, BACK, LEFT, RIGHT
};
class Camera
{
public:
	Camera(glm::vec3 position = glm::vec3(0.0f), float aspectRatio = 0.0f);
	void MouseMove(float xoffset, float yoffset, bool isConstantPitch = true);
	void KeyClick(KEYTYPE type, float deltaTime);
	void MouseScroll(float yoffset);
	glm::mat4 GetViewMatrix();
	glm::mat4 GetPerspectMatrix(const float& aspect);
	glm::vec3 GetPosition() const { return m_Position; }
private:
	glm::vec3 WorldUP{0.0f, 1.0f, 0.0f};
	float pitch = 0.0f;
	float yaw = -90.0f;
	glm::vec3 m_Front;
	glm::vec3 m_Right;
	glm::vec3 m_Up;
	glm::vec3 m_Position;

	float m_AspectRatio;
	float m_FOV = 45.0f;
	const static float MuseSentity;
	const static float speed;
private:
	void Recalculate();
};

