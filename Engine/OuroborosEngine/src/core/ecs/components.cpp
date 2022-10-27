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

	glm::quat rotation = glm::quat(rad_rot);
	glm::mat3 rotating_matrix{ glm::toMat4(rotation) };
	glm::vec3 front{ 0, 0, -1};
	front = rotating_matrix * front;
	glm::vec3 right = glm::normalize(glm::cross(front, rotating_matrix * glm::vec3{0, 1, 0}));
	glm::vec3 up = glm::cross(right, front);
	CalculateMatrices(eye, front, up);
}

void CameraComponent::CalculateMatrices(const glm::vec3& eye, const glm::vec3& front, const glm::vec3& up)
{
	perspective_matrix = glm::perspective(fov, width / height, near_plane, far_plane);
	perspective_matrix[1][1] *= -1;
	view_matrix = glm::lookAt(eye, eye +front, up);
}
