#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <iostream>
enum class KEYTYPE {
	LH_KEY_A,
	LH_KEY_D,
	LH_KEY_W,
	LH_KEY_S
};
class Camera
{
public:
	Camera(glm::vec3 position = { 0.0f,0.0f,0.0f });
	void KeyImpl(const KEYTYPE& keytype, const float& deltatime);
	void MouseImpl(float xoffset, float yoffset, GLboolean constraintPitch = true);
	void ZoomImpl(const float& yoffset);
	glm::mat4 getViewMatrix();
	glm::mat4 getViewPerspective(const float& width, const float& height);
	glm::mat4 getPerspective(const float& width, const float& height);
	glm::vec3 GetPosition();
	void SetYaw(float offset);

private:
	void reCalculate();
	glm::mat4 m_ViewMatrix;
	glm::vec3 m_Position;
	float m_Pitch;
	float m_Yaw = -90.0f;
	float m_Zoom = 45.0f;
	glm::vec3 m_Up;
	glm::vec3 m_Right;
	glm::vec3 m_Front;
	const static glm::vec3 WorldUp;

	const static float m_MouseSensitive;
	const static float m_KeyMoveSpeed;
};

