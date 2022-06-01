#pragma once
#include <glm.hpp>

struct Transform
{
	glm::vec3 pos;
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
	
};

struct Shader
{
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