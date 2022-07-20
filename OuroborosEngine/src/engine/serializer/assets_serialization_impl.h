#pragma once
#ifdef ENGINE
#include <yaml-cpp/yaml.h>
#include "../common/assets.h"
#include "shared_serialization_impl.h"

inline YAML::Node operator>>(YAML::Node node, Asset::MaterialData& material_data)
{
	node["albedo"]							>> material_data.albedo;
	node["metallic"]						>> material_data.metallic;
	node["roughness"]						>> material_data.roughness;
	node["ao"]								>> material_data.ao;
	node["has_albedo_texture"]				>> material_data.has_albedo_texture;
	node["has_normal_texture"]				>> material_data.has_normal_texture;
	node["has_metalroughness_texture"]		>> material_data.has_metalroughness_texture;
	node["has_ao_texture"]					>> material_data.has_ao_texture;
	node["has_metalic_texture"]				>> material_data.has_metalic_texture;
	node["has_roughness_texture"]			>> material_data.has_roughness_texture;
	node["has_emissive_texture"]			>> material_data.has_emissive_texture;
	node["is_roughness_texture_inverted"]	>> material_data.is_roughness_texture_inverted;
	return node;
}

inline YAML::Emitter& operator<<(YAML::Emitter& emitter, const Asset::MaterialData& material_data)
{
	emitter << YAML::Key << "albedo"						<< YAML::Value << material_data.albedo;
	emitter << YAML::Key << "metallic" << YAML::Value << material_data.metallic;
	emitter << YAML::Key << "roughness" << YAML::Value << material_data.roughness;
	emitter << YAML::Key << "ao" << YAML::Value << material_data.ao;
	emitter << YAML::Key << "has_albedo_texture" << YAML::Value << material_data.has_albedo_texture;
	emitter << YAML::Key << "has_normal_texture" << YAML::Value << material_data.has_normal_texture;
	emitter << YAML::Key << "has_metalroughness_texture" << YAML::Value << material_data.has_metalroughness_texture;
	emitter << YAML::Key << "has_ao_texture" << YAML::Value << material_data.has_ao_texture;
	emitter << YAML::Key << "has_metalic_texture" << YAML::Value << material_data.has_metalic_texture;
	emitter << YAML::Key << "has_roughness_texture" << YAML::Value << material_data.has_roughness_texture;
	emitter << YAML::Key << "has_emissive_texture" << YAML::Value << material_data.has_emissive_texture;
	emitter << YAML::Key << "is_roughness_texture_inverted" << YAML::Value << material_data.is_roughness_texture_inverted;
	return emitter;
}

#endif
