#include "components.h"

void TransformComponent::CalculateMatrix()
{
	rotation = glm::quat(glm::radians(euler_rotation));
	local_transform_matrix = glm::translate(glm::mat4(1.f), position) * glm::toMat4(rotation) * glm::scale(glm::mat4(1.f), scale);
}

