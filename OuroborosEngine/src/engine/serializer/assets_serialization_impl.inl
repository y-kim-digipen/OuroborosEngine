template<typename T,
	std::enable_if_t<std::is_same_v<T, Asset::MaterialData>, void*> = nullptr>
YAML::Emitter& _serialize(YAML::Emitter& emitter, T& material_data, void*)
{
	emitter << YAML::Key << "albedo" << YAML::Value << material_data.albedo;
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
