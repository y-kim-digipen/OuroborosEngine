#include <common/components.h>
#include "../engine.h"
void CameraComponent::SyncWithTransformComponent()
{
	auto& ecs_manager = OE::Engine::GetECSManager();
	OE::ecs_ID ownerID = ecs_manager.GetComponentOwner(this);
	assert(ecs_manager.HasComponent<TransformComponent>(ownerID));

	auto& transform_component = ecs_manager.GetComponent<TransformComponent>(ownerID);
	glm::vec3 eye = transform_component.GetPosition();
	glm::vec3 rot = transform_component.GetRotation();

	glm::vec3 rad_rot{ glm::radians(rot.x), glm::radians(rot.y), glm::radians(rot.z) };
	//transform_component.GetMatrix()
	float cos_yaw{ cos(rad_rot.y) };
	float sin_yaw{ sin(rad_rot.y) };
	float cos_pitch{ cos(rad_rot.x) };
	float sin_pitch{ sin(rad_rot.x) };
	//glm::vec4 front{ 0, 0, 1, 1 };
	//glm::vec3 front{ 0, 0, 1 };
	glm::vec3 front{ cos_yaw * cos_pitch, sin_pitch, sin_yaw * cos_pitch };
	//front = transform_component.GetRotationMatrix() * glm::vec4(front);
	//glm::vec3 front2 = glm::normalize(glm::vec3(front));
	glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3{ 0.f, 1.f, 0.f }));
	glm::vec3 up = glm::cross(right, front);
	CalculateMatrices(eye, front, up);
}

void CameraComponent::CalculateMatrices(const glm::vec3& eye, const glm::vec3& front, const glm::vec3& up)
{
	perspective_matrix = glm::perspective(fov, width / height, near_plane, far_plane);
	view_matrix = glm::lookAt(eye, eye + glm::vec3(0, 0, 1)/*front*/, up);
}
