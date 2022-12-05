#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include "../common/assets.h"


class Component
{
public:
	bool IsModified() const { return is_modified; }
protected:
	bool is_modified = false;
};

struct TransformComponent : public Component
{
public:
	TransformComponent() : TransformComponent(glm::vec3{}, glm::vec3{}, glm::vec3{ 1.f }) {}
	TransformComponent(const glm::vec3& pos, const glm::vec3& rotation, const glm::vec3& scale) : position(pos), rotation(rotation), scale(scale) { is_modified = true; }
	TransformComponent(const TransformComponent& other) : TransformComponent(other.GetPosition(), other.GetRotation(), other.GetScale()) {}
	TransformComponent(TransformComponent&& other) = default;
	TransformComponent& operator=(const TransformComponent& other) = default;
	TransformComponent& operator=(TransformComponent&& other) = default;

	glm::mat4 GetMatrix();

	const glm::vec3& GetPosition() const { return position; }
	void SetPosition(const glm::vec3& new_pos) { position = new_pos; is_modified = true; }
	const glm::vec3& GetRotation() const { return euler_rotation; }
	void SetRotation(const glm::vec3& new_rot) { euler_rotation = new_rot; is_modified = true; }
	const glm::vec3& GetScale() const { return scale; }
	void SetScale(const glm::vec3& new_scale) { scale = new_scale; is_modified = true; }
	void SetParentMatrix(const glm::mat4& matrix) { parent_matrix = matrix; is_modified = true; }
	const glm::mat4& GetParentMatrix() const { return parent_matrix; }
	const glm::mat4& GetLocalMatrix() {
		if (is_modified) { CalculateMatrix(); is_modified = false; } return local_transform_matrix;	}
private:
	void HierarchicalParentMatrixSet(uint16_t entityID, const glm::mat4& parent_matrix);

	void CalculateMatrix()
	{
		rotation = glm::quat(glm::radians(euler_rotation));
		local_transform_matrix = glm::translate(glm::mat4(1.f), position) * glm::toMat4(rotation) * glm::scale(glm::mat4(1.f), scale);
	}

	glm::vec3 position{ 0.f };
	glm::vec3 euler_rotation{ 0.f };
	glm::quat rotation{ 1.f, 0.0f, 0.f, 0.f };
	glm::vec3 scale{ 1.f };

	glm::mat4 parent_matrix{ 1.f };
	glm::mat4 local_transform_matrix { 0.0f };
};

struct CameraComponent : public Component
{
public:
	void SyncWithTransformComponent();
	glm::mat4 GetViewMatrix() const { return view_matrix; }
	glm::mat4 GetPerspectiveMatrix() const { return perspective_matrix; }
	bool IsUsing() const { return is_main_camera; }
	void SetUsing(bool use) { is_main_camera = use; }
	void SetCameraSize(uint16_t width, uint16_t height);

	glm::vec3 GetUpVector() const { return up; };
	glm::vec3 GetFrontVector() const { return right; };
	glm::vec3 GetRightVector() const { return front; };
private:
	void CalculateMatrices(const glm::vec3& eye, const glm::vec3& front, const glm::vec3& up);
	glm::mat4 view_matrix;
	glm::mat4 perspective_matrix;
	uint16_t width = 1600.f, height = 900.f;
	float near_plane = 0.1f, far_plane = 2500.f;
	float fov = glm::pi<float>() * 0.3f;

	glm::vec3 up, right, front;

	bool is_main_camera = false;
};

struct MeshComponent : Component
{
	std::string mesh_name;
};

struct MaterialComponent : Component
{
	std::string name ="material";
	Asset::MaterialData data{.albedo = {1.f,0.f,0.f}};
	bool flag = false;
	bool is_save = false;
	bool is_light = false;
	std::string texture_albedo_name = "";
	std::string texture_normal_name = "";
	std::string texture_metalroughness_name = "";
	std::string texture_ao_name = "";
	std::string texture_emissive_name = "";
	std::string texture_roughness_name = "";
};

struct ShaderComponent : Component
{
	std::string name = "shader_lightpass";
};


struct TagComponent : Component
{
	TagComponent() = default;
	TagComponent(const char* c) : tag{ c } {}
	TagComponent(const std::string& s) : tag{ s } {}
	TagComponent(std::string&& s) : tag{std::move(s) } {}
	TagComponent(const TagComponent& other) = default;
	TagComponent(TagComponent&& other) = default;
	TagComponent& operator = (const TagComponent& other) = default;
	TagComponent& operator = (TagComponent&& other) = default;
	std::string tag;
};

struct LightComponent : Component
{
	bool init = false;
	Asset::LightData data;
};

struct ScriptComponent : Component
{
	std::string name;
};
