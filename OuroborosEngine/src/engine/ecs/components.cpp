#include "components.h"
#include "../engine.h"

void TransformComponent::CalculateMatrix()
{
	rotation = glm::quat(glm::radians(euler_rotation));
	local_transform_matrix = glm::translate(glm::mat4(1.f), position) * glm::toMat4(rotation) * glm::scale(glm::mat4(1.f), scale);
}

//CameraComponent::CameraComponent(glm::vec3 eye, glm::vec3 back, glm::vec3 up, float near, float far, float fov)
//{
//}

void CameraComponent::SyncWithTransformComponent()
{
	ECS_Manager& ecs_manager = OE::Engine::GetECSManager();
	OE::ecs_ID owner = ecs_manager.GetComponentOwner(this);

	//Shouldn't be happen
	assert(ecs_manager.HasComponent<TransformComponent>(owner));

	const TransformComponent& transform_component = ecs_manager.GetComponent<TransformComponent>(owner);
	glm::vec3 eye = transform_component.GetPosition();

	const float yaw_as_radian	= glm::radians(transform_component.GetRotation().z);
	const float pitch_as_radian	= glm::radians(transform_component.GetRotation().x);
	//const float roll_as_radian	= glm::radians(transform_component.GetRotation().z);
	const float cos_yaw = glm::cos(yaw_as_radian);
	const float sin_yaw = glm::sin(yaw_as_radian);
	const float cos_pitch = glm::cos(pitch_as_radian);
	const float sin_pitch = glm::sin(pitch_as_radian);

	glm::vec3 front
	{	cos_yaw * cos_pitch,
		sin_pitch,
		sin_yaw * cos_pitch
	};
	const glm::vec3 world_up{ 0, 1, 0 };
	glm::vec3 right = glm::normalize(glm::cross(front, world_up));
	glm::vec3 up = glm::normalize(glm::cross(right, front));
	CalculateMatrix(eye, front, up);
}

glm::mat4 CameraComponent::GetPerspectiveMatrix()
{
	return perspective_matrix;
}

glm::mat4 CameraComponent::GetViewAtMatrix()
{
	return view_matrix;
}

void CameraComponent::SetAsMainCamera()
{
	is_main_camera = true;
}

void CameraComponent::CalculateMatrix(const glm::vec3& eye, const glm::vec3& front, const glm::vec3& up)
{
	perspective_matrix = glm::perspective(fov, width / height, near_plane, far_plane);
	view_matrix = glm::lookAt(eye, eye + front, up);
}


