#pragma once
#include <glm.hpp>

struct TransformComponent
{
	glm::vec3 pos;
	glm::vec3 scale= glm::vec3(1.f);
	glm::vec3 rotate_axis = {1.0f, 0.f,0.f};
	float angle =0.f;
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
	Renderer::MaterialData data;
	bool flag = false;
	bool is_save = false;
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
	glm::vec3 position;
	float cutoff;
	glm::vec3 diffuse;
	float out_cutoff;
	glm::vec3 ambient;
	float falloff;
	glm::vec3 specular;
	float padding4;
	glm::vec3 direction;
	int light_type;
};

//void DrawSystem(mesh, sahder)
//{
//	Redenrer->Render(mesh, shader, transform);
//};
//
//ResourceManager
//{
//	vector<image*>
//	vector<file* >
//}

//struct 