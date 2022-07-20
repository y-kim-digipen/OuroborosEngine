#ifndef COMPONENTS_H
#define COMPONENTS_H
#include <glm.hpp>
#include <gtc/vec1.hpp>
#include "../common/assets.h"

struct TransformComponent
{
	glm::vec3 pos;
	glm::vec3 scale= glm::vec3(1.f);
	glm::vec3 rotation = {1.0f, 0.f,0.f};
};

struct VelocityComponent
{
	glm::vec3 vel;
};

struct LifeTimeComponent
{
	float life_time;
};

struct MeshComponent
{
	std::string mesh_name;
};

struct MaterialComponent
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

struct ShaderComponent
{
	std::string name ="shader";
};

struct BoolWrapperComponent
{
	bool bool_type;
};

struct TagComponent
{
	std::string tag;
};

struct LightComponent
{
	bool init = false;
	Asset::LightData data;
};

struct ScriptComponent
{
	std::string name;
};
#endif //!COMPONENTS_H