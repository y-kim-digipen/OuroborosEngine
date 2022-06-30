#include "camera.h"
#include <gtc/matrix_transform.hpp>


namespace Renderer
{
	Camera::Camera(glm::vec3 position, glm::vec3 up_, float yaw, float pitch)
	:front(glm::vec3(0.0f, 0.0f, -1.0f)), movement_speed(SPEED), mouse_sensitivity(SENSITIVITY), zoom(ZOOM)
	{
		data.position = position;
		world_up = up_;
		Camera::yaw = yaw;
		Camera::pitch = pitch;
		updateCameraVectors();
	}

	Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
		:front(glm::vec3(0.0f, 0.0f, -1.0f)), movement_speed(SPEED), mouse_sensitivity(SENSITIVITY), zoom(ZOOM)
	{
		data.position = glm::vec3(posX, posY, posZ);
		world_up = glm::vec3(upX, upY, upZ);
		Camera::yaw = yaw;
		Camera::pitch = pitch;
		updateCameraVectors();
	}

	void Camera::KeyboardInput(Camera_MoveTo direction, double deltaTime)
	{
		float velocity = movement_speed * static_cast<float>(deltaTime);
		if (direction == Camera_MoveTo::FORWARD)
			data.position += front * velocity;
		if (direction == Camera_MoveTo::BACKWARD)
			data.position -= front * velocity;
		if (direction == Camera_MoveTo::LEFT)
			data.position -= right * velocity;
		if (direction == Camera_MoveTo::RIGHT)
			data.position += right * velocity;
		if (direction == Camera_MoveTo::UP)
			data.position += up * velocity;
		if (direction == Camera_MoveTo::DOWN)
			data.position -= up * velocity;
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

		Camera::front = glm::normalize(front);
		right = glm::normalize(glm::cross(Camera::front, world_up));
		up = glm::normalize(glm::cross(right, Camera::front));
	}

	glm::mat4 Camera::GetCameraMat()
	{
		return glm::lookAt(data.position, data.position + front, up);
	}

	void Camera::ResetCamera(glm::vec3 campos)
	{
		data.position = campos;
		world_up = glm::vec3(0.0f, 1.0f, 0.0f);
		yaw = -90.f;
		pitch = 0.f;
	}
}
