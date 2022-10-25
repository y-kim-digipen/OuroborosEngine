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

	const glm::mat4& GetMatrix()
	{
		if (is_modified) { CalculateMatrix(); is_modified = false; }
		return local_transform_matrix;
	}

	const glm::vec3& GetPosition() const { return position; }
	void SetPosition(const glm::vec3& new_pos) { position = new_pos; is_modified = true; }
	const glm::vec3& GetRotation() const { return euler_rotation; }
	void SetRotation(const glm::vec3& new_rot) { euler_rotation = new_rot; is_modified = true; }
	const glm::vec3& GetScale() const { return scale; }
	void SetScale(const glm::vec3& new_scale) { scale = new_scale; is_modified = true; }
private:
	void CalculateMatrix();
	glm::vec3 position{ 0.f };
	glm::vec3 euler_rotation{ 0.f };
	glm::quat rotation{ 1.f, 0.0f, 0.f, 0.f };
	glm::vec3 scale{ 1.f };

	glm::mat4 transform_matrix{ 0.0f };
	glm::mat4 local_transform_matrix { 0.0f };
};

struct CameraComponent : Component
{
public:
	//CameraComponent() = default;
	//CameraComponent(glm::vec3 eye, glm::vec3 back, glm::vec3 up, float near, float far, float fov = 0);
	void SyncWithTransformComponent();
	glm::mat4 GetPerspectiveMatrix();
	glm::mat4 GetViewAtMatrix();
	void SetAsMainCamera();
private:
	void CalculateMatrix(const glm::vec3& eye, const glm::vec3& right, const glm::vec3& up);

	float fov = glm::pi<float>() * 0.3;
	float near_plane = 0.1f, far_plane = 50.f;
	float width = 1600, height = 900;

	glm::mat4 view_matrix{};
	glm::mat4 perspective_matrix{};
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
