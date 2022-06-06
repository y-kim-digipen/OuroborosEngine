#include "camera.h"
#include <gtc/matrix_transform.hpp>

namespace Renderer
{
	Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
	:front(glm::vec3(0.0f, 0.0f, -1.0f)), movement_speed(SPEED), mouse_sensitivity(SENSITIVITY), zoom(ZOOM)
	{
		position = position;
		world_up = up;
		yaw = yaw;
		pitch = pitch;
	}

	Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
		:front(glm::vec3(0.0f, 0.0f, -1.0f)), movement_speed(SPEED), mouse_sensitivity(SENSITIVITY), zoom(ZOOM)
	{
		camera_data.position = glm::vec3(posX, posY, posZ);
		world_up = glm::vec3(upX, upY, upZ);
		yaw = yaw;
		pitch = pitch;
		updateCameraVectors();
	}

	void Camera::KeyboardInput(Camera_MoveTo direction, double deltaTime)
	{
		float velocity = movement_speed * static_cast<float>(deltaTime);
		if (direction == Camera_MoveTo::FORWARD)
			camera_data.position += front * velocity;
		if (direction == Camera_MoveTo::BACKWARD)
			camera_data.position -= front * velocity;
		if (direction == Camera_MoveTo::LEFT)
			camera_data.position -= right * velocity;
		if (direction == Camera_MoveTo::RIGHT)
			camera_data.position += right * velocity;
		if (direction == Camera_MoveTo::UP)
			camera_data.position += up * velocity;
		if (direction == Camera_MoveTo::DOWN)
			camera_data.position -= up * velocity;
	}

	void Camera::MouseInput(float xoffset, float yoffset, bool constrainPitch)
	{
		xoffset *= mouse_sensitivity;
		yoffset *= mouse_sensitivity;

		yaw += xoffset;
		pitch += yoffset;

		if (constrainPitch)
		{
			if (pitch > 89.0f)
				pitch = 89.0f;
			if (pitch < -89.0f)
				pitch = -89.0f;
		}
		updateCameraVectors();
	}

	void Camera::MouseScrollInput(float yoffset)
	{
		zoom -= (float)yoffset;
		if (zoom < 1.0f)
			zoom = 1.0f;
		if (zoom > 45.0f)
			zoom = 45.0f;
	}

	void Camera::updateCameraVectors()
	{
		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		front = glm::normalize(front);
		right = glm::normalize(glm::cross(front, world_up));
		up = glm::normalize(glm::cross(right, front));
	}

	glm::mat4 Camera::GetCameraMat()
	{
		return glm::lookAt(camera_data.position, camera_data.position + front, up);
	}

	void Camera::ResetCamera(glm::vec3 campos)
	{
		camera_data.position = campos;
		world_up = glm::vec3(0.0f, 1.0f, 0.0f);
		yaw = -90.f;
		pitch = 0.f;
	}
}
