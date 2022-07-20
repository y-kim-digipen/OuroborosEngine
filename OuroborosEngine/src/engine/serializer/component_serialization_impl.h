#pragma once
//===============
#ifdef ENGINE
#include <../src/engine/ecs/components.h>
#include <yaml-cpp/yaml.h>
#include "shared_serialization_impl.h"
#include "assets_serialization_impl.h"

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
	emitter << YAML::Key << "name"							<< YAML::Value << material.name;
	emitter << YAML::Key << "material_data"					<< YAML::Value << YAML::BeginMap << material.data << YAML::EndMap;
	emitter << YAML::Key << "flag"							<< YAML::Value << material.flag;
	emitter << YAML::Key << "is_save"						<< YAML::Value << material.is_save;
	emitter << YAML::Key << "is_light"						<< YAML::Value << material.is_light;
	emitter << YAML::Key << "texture_albedo_name"			<< YAML::Value << material.texture_albedo_name;
	emitter << YAML::Key << "texture_normal_name"			<< YAML::Value << material.texture_normal_name;
	emitter << YAML::Key << "texture_metalroughness_name"	<< YAML::Value << material.texture_metalroughness_name;
	emitter << YAML::Key << "texture_ao_name"				<< YAML::Value << material.texture_ao_name;
	emitter << YAML::Key << "texture_emissive_name"			<< YAML::Value << material.texture_emissive_name;
	emitter << YAML::Key << "texture_roughness_name"		<< YAML::Value << material.texture_roughness_name;
	return emitter;
}

inline YAML::Node operator>>(YAML::Node node, MaterialComponent& material)
{
	node["name"]								>> material.name;
	node["material_data"]						>> material.data;
	node["flag"]								>> material.flag;
	node["is_save"]								>> material.is_save;
	node["is_light"]							>> material.is_light;
	node["texture_albedo_name"]					>> material.texture_albedo_name;
	node["texture_normal_name"]					>> material.texture_normal_name;
	node["texture_metalroughness_name"]			>> material.texture_metalroughness_name;
	node["texture_ao_name"]						>> material.texture_ao_name;
	node["texture_emissive_name"]				>> material.texture_emissive_name;
	node["texture_roughness_name"]				>> material.texture_roughness_name;

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
	emitter << YAML::Key << "init" << YAML::Value << false;
	emitter << YAML::Key << "data.pos" << YAML::Value << light.data.pos;
	emitter << YAML::Key << "data.cutoff" << YAML::Value << light.data.cutoff;
	emitter << YAML::Key << "data.diffuse" << YAML::Value << light.data.diffuse;
	emitter << YAML::Key << "data.out_cutoff" << YAML::Value << light.data.out_cutoff;
	emitter << YAML::Key << "data.dir" << YAML::Value << light.data.dir;
	emitter << YAML::Key << "data.falloff" << YAML::Value << light.data.falloff;
	emitter << YAML::Key << "data.type" << YAML::Value << light.data.type;
	return emitter;
}

inline YAML::Node operator>>(YAML::Node node, LightComponent& light)
{
	node["init"] >> light.init;
	node["data.pos"] >> light.data.pos;
	node["data.cutoff"] >> light.data.cutoff;
	node["data.diffuse"] >> light.data.diffuse;
	node["data.out_cutoff"] >> light.data.out_cutoff;
	node["data.dir"] >> light.data.dir;
	node["data.falloff"] >> light.data.falloff;
	node["data.type"] >> light.data.type;
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