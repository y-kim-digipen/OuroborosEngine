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

void CameraComponent::SetCameraSize(uint16_t width, uint16_t height)
{
	this->width = width;
	this->height = height;
	is_modified = true;
}

void CameraComponent::CalculateMatrices(const glm::vec3& eye, const glm::vec3& front, const glm::vec3& up)
{
	float aspect_ratio = static_cast<float>(width) / height;
	float focal_length = 1 / std::tan(fov * 0.5f);

	perspective_matrix = glm::mat4(1.0f);
	perspective_matrix[0][0] = focal_length / aspect_ratio;
	perspective_matrix[1][1] = -focal_length;
	perspective_matrix[2][2] = near_plane / (far_plane - near_plane);
	perspective_matrix[2][3] = -1.0f;
	perspective_matrix[3][2] = (far_plane * near_plane) / (far_plane - near_plane);
	perspective_matrix[3][3] = 0.0f;

	//perspective_matrix = glm::perspective(fov, static_cast<float>(width) / height , far_plane, near_plane);
	//perspective_matrix[1][1] *= -1;
	view_matrix = glm::lookAt(eye, eye +front, up);
}

glm::mat4 TransformComponent::GetMatrix()
{
	if (is_modified) { 
		auto owner = OE::Engine::ecs_manager.GetComponentOwner<TransformComponent>(this);
		auto entity = OE::Engine::ecs_manager.GetEntity(owner);
		CalculateMatrix(); 
		is_modified = false; 
		if (entity.hierarchy.HasParent() == false)
		{
			HierarchicalParentMatrixSet(owner, glm::mat4{ 1.f });
		}
	}
	return parent_matrix * local_transform_matrix;
}

void TransformComponent::HierarchicalParentMatrixSet(uint16_t entityID, const glm::mat4& parent_matrix)
{
	decltype(OE::Engine::ecs_manager)::Entity& entity = OE::Engine::ecs_manager.GetEntity(entityID);
	if (OE::Engine::ecs_manager.HasComponent<TransformComponent>(entityID))
	{
		auto& transform_component = OE::Engine::ecs_manager.GetComponent<TransformComponent>(entityID);
		transform_component.SetParentMatrix(parent_matrix);
		glm::mat4 this_matrix = transform_component.GetLocalMatrix() * parent_matrix;
		for (auto childID : entity.hierarchy.GetChild())
		{
			HierarchicalParentMatrixSet(childID, this_matrix);
		}
	}
};