#pragma once
#include <glm.hpp>
#include <gtc/vec1.hpp>

struct Transform
{
	glm::vec3 pos;
	glm::vec3 scale= glm::vec3(1.f);
	glm::vec3 rotate_axis = {1.0f, 0.f,0.f};
	glm::vec1 angle{ 0.f };
};


struct Velocity
{
	glm::vec3 vel;
};

struct LifeTime
{
	float life_time;
};

struct Mesh
{
	std::string mesh_name;
};

struct Material
{
	std::string material_name;
};

struct Shader
{
	std::string shader_name;
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