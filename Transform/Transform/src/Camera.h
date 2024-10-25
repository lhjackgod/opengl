#pragma once
#include<glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
namespace CameraSpace {

	enum class CameraMovement {
		FOWRAD,
		BACKWARD,
		LEFT,
		RIGHT
	};
	class Camera {
	public:
		glm::vec3 Position;
		glm::vec3 Up;
		glm::vec3 Right;
		glm::vec3 Front;
		glm::vec3 WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
		float yaw;
		float patch;
		glm::mat4 view;
		float MovementSpeed;
		float MouseSensitive;
		float fov = 45.0f;
		Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f,0.0f), glm::vec3 front = glm::vec3(0.0f,0.0f,-1.0f), float yaw = glm::radians(-90.0f), float patch = glm::radians(0.0f),
			float movementSpeed = 2.5f, float mousesensitivity = 0.001f)
			: Position(position), Up(glm::vec3(0.0f,1.0f,0.0f)), yaw(yaw), patch(patch), Front(front),
		MovementSpeed(movementSpeed), MouseSensitive(mousesensitivity)
		{

			updateView();
		}
		glm::mat4 GetViewMatrix() {
			view = glm::lookAt(Position, Front + Position, Up);
			return view;
		}
		void ProcessKeyboard(CameraMovement direction, float deltaTime) {
			float velocity = deltaTime * MovementSpeed;
			if (direction == CameraMovement::FOWRAD) {
				Position += velocity * Front;
			}
			if (direction == CameraMovement::BACKWARD) {
				Position -= velocity * Front;
			}
			if (direction == CameraMovement::LEFT) {
				Position -= velocity * Right;
			}
			if (direction == CameraMovement::RIGHT) {
				Position += velocity * Right;
			}
		}
		void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true) {
			xoffset *= MouseSensitive;
			yoffset *= MouseSensitive;
			yaw += xoffset;
			patch += yoffset;
			if (constrainPitch) {
				if (patch > 89.0f) {
					patch = 89.0f;
				}
				if (patch < -89.0f) {
					patch = -89.0f;
				}
			}
			updateView();
		}
		void ProcessMouseScrollback(float yoffset) {
			fov += yoffset;
			if (fov < 0.0f) fov = 0.0f;
			if (fov > 90.0f) fov = 90.0f;
		}
	private:
		void updateView() {
			glm::vec3 lookPoint = Position + Front;
			lookPoint.x += (static_cast<float>(std::cos(patch) * std::cos(yaw)));
			lookPoint.y += (static_cast<float>(std::sin(patch)));
			lookPoint.z += (static_cast<float>(std::sin(yaw) * std::cos(patch)));
			Front = lookPoint - Position;
			Front = glm::normalize(Front);
			Right = glm::cross(Front, WorldUp);
			Up = glm::cross(Right, Front);
		}
	};
}

