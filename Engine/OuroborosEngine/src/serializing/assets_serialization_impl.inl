template<typename T,
	std::enable_if_t<std::is_same_v<T, Asset::Material>, void*> = nullptr>
YAML::Emitter& _serialize(YAML::Emitter& emitter, T& material_data, void*)
{
	bool use_texture = material_data.pbr_mat.GetMode() == Shared::MaterialMode::TEXTURE;
	emitter << YAML::Key << "use_texture" << YAML::Value << use_texture;

	if (!use_texture)
	{
		Shared::PBR_NONE_TEXTURE& non_texture = *material_data.pbr_mat.structure.non_texture;
		emitter << YAML::Key << "albedo" << YAML::Value;
		_serialize(emitter, non_texture.albedo, nullptr);
		//emitter << YAML::Key << "albedo" << YAML::Value << non_texture.albedo;
		emitter << YAML::Key << "metallic" << YAML::Value << non_texture.metallic;
		emitter << YAML::Key << "roughness" << YAML::Value << non_texture.roughness;
		emitter << YAML::Key << "ao" << YAML::Value << non_texture.ao;
	}
	else
	{
		Shared::PBR_TEXTURE& texture = *material_data.pbr_mat.structure.texture;
		const bool combine_metallic_roughness = texture.use_combined_metallic_roughness;
		emitter << YAML::Key << "is_smoothness" << YAML::Value << texture.is_smoothness;
		emitter << YAML::Key << "use_combined_metallic_roughness" << YAML::Value << combine_metallic_roughness;

		emitter << YAML::Key << "albedo" << YAML::Value << std::string(texture.texture_names[Shared::ALBEDO]);
		emitter << YAML::Key << "normal" << YAML::Value << std::string(texture.texture_names[Shared::NORMAL]);
		if (combine_metallic_roughness)
		{
			emitter << YAML::Key << "metallic_roughness" << YAML::Value << std::string(texture.texture_names[Shared::METALLIC_ROUGHNESS]);
		}
		else
		{
			emitter << YAML::Key << "metallic" << YAML::Value << std::string(texture.texture_names[Shared::METALLIC]);
			emitter << YAML::Key << "roughness" << YAML::Value << std::string(texture.texture_names[Shared::ROUGHNESS]);
		}
		emitter << YAML::Key << "ao" << YAML::Value << std::string(texture.texture_names[Shared::AO]);
		emitter << YAML::Key << "emissive" << YAML::Value << std::string(texture.texture_names[Shared::EMISSIVE]);
	}
	return emitter;
}
