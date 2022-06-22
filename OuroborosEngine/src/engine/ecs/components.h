#ifndef COMPONENTS_H
#define COMPONENTS_H
#include <glm.hpp>
#include <gtc/vec1.hpp>
#include "../common/assets.h"

#include <rttr/type>

template<typename TComponent>
void RTTRComponent()
{
	//static_assert(true, "rttr reflection not implemented");
};

struct TransformComponent
{
	glm::vec3 pos;
	glm::vec3 scale= glm::vec3(1.f);
	glm::vec3 rotate_axis = {1.0f, 0.f,0.f};
	float angle{ 0.f };
	
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
	Asset::MaterialData data{.ambient = {1.f,0.f,0.f}};
	bool flag = false;
	bool is_save = false;
	bool is_light = false;
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

#endif //!COMPONENTS_H