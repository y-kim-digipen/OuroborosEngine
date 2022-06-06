#pragma once
#include <glm.hpp>

struct TransformComponent
{
	glm::vec3 pos;
	glm::vec3 scale= glm::vec3(1.f);
	glm::vec3 rotate_axis = {1.0f, 0.f,0.f};
	float angle =0.f;
};


struct Velocity
{
	glm::vec3 vel;
};

struct LifeTime
{
	float life_time;
};

struct MeshComponent
{
	std::string mesh_name;
};

struct MaterialComponent
{
	std::string name;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
};

struct ShaderComponent
{
	std::string name;
};

struct BoolWrapper
{
	bool bool_type;
};

struct Tag
{
	std::string tag;
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