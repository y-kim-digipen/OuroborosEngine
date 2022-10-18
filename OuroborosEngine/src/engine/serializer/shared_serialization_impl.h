#pragma once

#ifdef ENGINE
#include <glm.hpp>
#include <yaml-cpp/yaml.h>
#include <iostream>

#include "../ecs/components.h"
#include "../common/assets.h"

namespace _serialization_impl
{
	//arithmatic or string values
	template<typename T,
		std::enable_if_t<std::is_arithmetic_v<T>, void*> = nullptr>
	YAML::Emitter& _serialize(YAML::Emitter& emitter, T& val, void*)
	{
		emitter << val;
		return emitter;
	}

	template<typename T,
		std::enable_if_t<std::is_same_v<T, std::string>, void*> = nullptr>
	YAML::Emitter& _serialize(YAML::Emitter& emitter, T& val, void*)
	{
		emitter.Write(val);
		return emitter;
	}

	//glm::vec3
	template<typename T,
		std::enable_if_t<std::is_same_v<T, glm::vec3>, void*> = nullptr>
	YAML::Emitter& _serialize(YAML::Emitter& emitter, T& val, void*)
	{
		emitter << YAML::Flow << YAML::BeginSeq << val.x << val.y << val.z << YAML::EndSeq;
		return emitter;
	}

#include "assets_serialization_impl.inl"
#include "component_serialization_impl.inl"

	//Default
	template<typename T>
	YAML::Emitter& _serialize(YAML::Emitter& emitter, T& val, ...)
	{
		std::cout << "Serialization implementation for " << typeid(T).name() << " does not exist!" << std::endl;
		return emitter;
	}
}

namespace _deserialization_impl
{
	//arithmatic or string values
	template<typename T,
		std::enable_if_t<std::is_arithmetic_v<T> || std::is_same_v<T, std::string>, void*> = nullptr>
	YAML::Node _deserialize(YAML::Node node, T& val, void*)
	{
		val = node.as<T>();
		return node;
	}

	template<typename T,
		std::enable_if_t<std::is_same_v<T, const char*>, void*> = nullptr>
	YAML::Node _deserialize(YAML::Node node, T& val, void*)
	{
		return _deserialize(node, std::string(val), nullptr);
	}

	//glm::vec3
	template<typename T,
		std::enable_if_t<std::is_same_v<T, glm::vec3>, void*> = nullptr>
	YAML::Node _deserialize(YAML::Node node, T& val, void*)
	{
		YAML::iterator it = node.begin();
		val.x = it++->as<float>();
		val.y = it++->as<float>();
		val.z = it->as<float>();
		return node;
	}

#include "assets_deserialization_impl.inl"
#include "component_deserialization_impl.inl"

	//Default
	template<typename T>
	YAML::Node _deserialize(YAML::Node node, T& val, ...)
	{
		std::cout << "Deserialization implementation for " << typeid(T).name() << " does not exist!" << std::endl;
		return node;
	}
}

template<typename T>
YAML::Node operator >> (YAML::Node node, T& t)
{
	return _deserialization_impl::_deserialize(node, t, nullptr);
}

template<typename T>
YAML::Emitter& operator << (YAML::Emitter& emitter, T& t)
{
	return _serialization_impl::_serialize(emitter, t, nullptr);
}
#endif
