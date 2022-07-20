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
//===============
#ifdef ENGINE
#include <yaml-cpp/yaml.h>

inline YAML::Emitter& operator << (YAML::Emitter& emitter, const glm::vec3& vec3)
{
	emitter << YAML::Flow << YAML::BeginSeq << vec3.x << vec3.y << vec3.z << YAML::EndSeq;
	return emitter;
}

inline YAML::Node operator >> (YAML::Node node, glm::vec3& vec3)
{
	YAML::iterator it = node.begin();
	vec3.x = it++->as<float>();
	vec3.y = it++->as<float>();
	vec3.z = it->as<float>();
	return node;
}

template<typename T>
inline YAML::Node operator >> (YAML::Node node, T& val)
{
	val = node.as<T>();
	return node;
}

//inline YAML::Node operator >> (YAML::Node node, int& i)
//{
//	YAML::iterator it = node.begin();
//	i = it->as<int>();
//	return node;
//}
//
//inline YAML::Node operator >> (YAML::Node node, float& f)
//{
//	YAML::iterator it = node.begin();
//	f = it->as<float>();
//	return node;
//}
//
//
//inline YAML::Node operator >> (YAML::Node node, bool& b)
//{
//	YAML::iterator it = node.begin();
//	b = it->as<std::string>();
//	return node;
//}

//inline YAML::Node operator >> (YAML::Node node, std::string& s)
//{
//	YAML::iterator it = node.begin();
//	s = it->as<std::string>();
//	return node;
//}

inline YAML::Emitter& operator<<(YAML::Emitter& emitter, const TransformComponent& transform)
{

	emitter << YAML::Key << "pos" << YAML::Value << transform.pos;
	emitter << YAML::Key << "scale" << YAML::Value << transform.scale;
	emitter << YAML::Key << "rotation" << YAML::Value << transform.rotation;
	return emitter;
}

inline YAML::Node operator>>(YAML::Node node, TransformComponent& transform)
{
	node["pos"] >> transform.pos;
	node["scale"] >> transform.scale;
	node["rotation"] >> transform.rotation;
	return node;
}

inline YAML::Emitter& operator<<(YAML::Emitter& emitter, const VelocityComponent& velocity)
{
	emitter << YAML::Key << "vel" << YAML::Value << velocity.vel;
	return emitter;
}

inline YAML::Node operator>>(YAML::Node node, VelocityComponent& velocity)
{
	node["vel"] >> velocity.vel;
	return node;
}

inline YAML::Emitter& operator<<(YAML::Emitter& emitter, const LifeTimeComponent& lifetime)
{
	emitter << YAML::Key << "life_time" << YAML::Value << lifetime.life_time;
	return emitter;
}

inline YAML::Node operator>>(YAML::Node node, LifeTimeComponent& life_time)
{
	node["life_time"] >> life_time.life_time;
	return node;
}

inline YAML::Emitter& operator<<(YAML::Emitter& emitter, const MeshComponent& mesh)
{
	emitter << YAML::Key << "mesh_name" << YAML::Value << mesh.mesh_name;
	return emitter;
}

inline YAML::Node operator>>(YAML::Node node, MeshComponent& mesh)
{
	node["mesh_name"] >> mesh.mesh_name;
	return node;
}

inline YAML::Emitter& operator<<(YAML::Emitter& emitter, const MaterialComponent& material)
{
	return emitter;
}

inline YAML::Node operator>>(YAML::Node node, MaterialComponent& material)
{
	return node;
}

inline YAML::Emitter& operator<<(YAML::Emitter& emitter, const ShaderComponent& shader)
{
	emitter << YAML::Key << "name" << YAML::Value << shader.name;
	return emitter;
}

inline YAML::Node operator>>(YAML::Node node, ShaderComponent& shader)
{
	node["name"] >> shader.name;
	return node;
}

inline YAML::Emitter& operator<<(YAML::Emitter& emitter, const BoolWrapperComponent& bool_wrapper)
{
	emitter << YAML::Key << "bool_type" << YAML::Value << bool_wrapper.bool_type;
	return emitter;
}

inline YAML::Node operator>>(YAML::Node node, BoolWrapperComponent& bool_wrapper)
{
	node["bool_type"] >> bool_wrapper.bool_type;
	return node;
}

inline YAML::Emitter& operator<<(YAML::Emitter& emitter, const TagComponent& tag)
{
	emitter << YAML::Key << "tag" << YAML::Value << tag.tag;
	return emitter;
}

inline YAML::Node operator>>(YAML::Node node, TagComponent& tag)
{
	node["tag"] >> tag.tag;
	return node;
}

inline YAML::Emitter& operator<<(YAML::Emitter& emitter, const LightComponent& light)
{
	return emitter;
}

inline YAML::Node operator>>(YAML::Node node, LightComponent& light)
{
	return node;
}

inline YAML::Emitter& operator<<(YAML::Emitter& emitter, const ScriptComponent& script)
{
	emitter << YAML::Key << "name" << YAML::Value << script.name;
	return emitter;
}

inline YAML::Node operator>>(YAML::Node node, ScriptComponent& script)
{
	node["name"] >> script.name;
	return node;
}

#endif //ENGINE
#endif //!COMPONENTS_H