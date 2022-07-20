#pragma once

#ifdef ENGINE
#include <glm.hpp>
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
#endif
