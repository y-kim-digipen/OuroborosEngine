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
	std::string mesh_name ="suzanne";
};

struct MaterialComponent
{
	std::string name ="material";
	Renderer::MaterialData data{.ambient = {1.f,0.f,0.f}};
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
	bool init = false;
	Renderer::LightData data;
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