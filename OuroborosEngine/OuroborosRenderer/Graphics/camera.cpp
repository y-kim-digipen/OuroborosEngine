#include "camera.h"
#include <gtc/matrix_transform.hpp>

namespace Renderer
{
	Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
	:Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
	}

	Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
		:Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = glm::vec3(posX, posY, posZ);
		WorldUp = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	void Camera::KeyboardInput(Camera_MoveTo direction, double deltaTime)
	{
		float velocity = MovementSpeed * static_cast<float>(deltaTime);
		if (direction == Camera_MoveTo::FORWARD)
			Position += Front * velocity;
		if (direction == Camera_MoveTo::BACKWARD)
			Position -= Front * velocity;
		if (direction == Camera_MoveTo::LEFT)
			Position -= Right * velocity;
		if (direction == Camera_MoveTo::RIGHT)
			Position += Right * velocity;
		if (direction == Camera_MoveTo::UP)
			Position += Up * velocity;
		if (direction == Camera_MoveTo::DOWN)
			Position -= Up * velocity;
	}

	void Camera::MouseInput(float xoffset, float yoffset, bool constrainPitch)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw += xoffset;
		Pitch += yoffset;

		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}
		updateCameraVectors();
	}

	void Camera::MouseScrollInput(float yoffset)
	{
		Zoom -= (float)yoffset;
		if (Zoom < 1.0f)
			Zoom = 1.0f;
		if (Zoom > 45.0f)
			Zoom = 45.0f;
	}

	void Camera::updateCameraVectors()
	{
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		Right = glm::normalize(glm::cross(Front, WorldUp));
		Up = glm::normalize(glm::cross(Right, Front));
	}

	glm::mat4 Camera::GetCameraMat()
	{
		return glm::lookAt(Position, Position + Front, Up);
	}

	void Camera::ResetCamera(glm::vec3 campos)
	{
		Position = campos;
		WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
		Yaw = -90.f;
		Pitch = 0.f;
	}
}
