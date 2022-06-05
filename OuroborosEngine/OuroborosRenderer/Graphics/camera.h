#ifndef CAMERA_H
#define CAMERA_H
#include <glm.hpp>

namespace Renderer
{
	enum class Camera_MoveTo
	{
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT,
		UP,
		DOWN,
	};

	constexpr float YAW = -90.0f;
	constexpr float PITCH = 0.0f;
	constexpr float SPEED = 10.5f;
	constexpr float SENSITIVITY = 10.f;
	constexpr float ZOOM = 45.0f;

	class Camera
	{
	public:
		Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
			float yaw = YAW, float pitch = PITCH);

		Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

		void KeyboardInput(Camera_MoveTo direction, double deltaTime);
		void MouseInput(float xoffset, float yoffset, bool constrainPitch = true);
		void MouseScrollInput(float yoffset);
		void updateCameraVectors();
		glm::mat4 GetCameraMat();
		void ResetCamera(glm::vec3 campos = glm::vec3(0.0f, 0.0f, 0.0f));

		glm::vec3 Position;
		glm::vec3 Front;
		glm::vec3 Up;
		glm::vec3 Right;
		glm::vec3 WorldUp;

		float Yaw;
		float Pitch;

		float MovementSpeed;
		float MouseSensitivity;
		float Zoom;
	};


}





#endif
