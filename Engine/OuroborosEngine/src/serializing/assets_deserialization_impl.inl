template<typename T,
	std::enable_if_t<std::is_same_v<T, Asset::Material>, void*> = nullptr>
YAML::Node _deserialize(YAML::Node node, T& material_data, void*)
{
	Asset::Material& material = material_data;
	bool valid = node.IsMap();
	YAML::iterator itr = node.begin();

	assert(itr->first.as<std::string>() == "use_texture");
	const bool use_texture = (itr++)->second.as<bool>();
	material.pbr_mat.SwitchMode(use_texture ? Shared::MaterialMode::TEXTURE : Shared::MaterialMode::NONE_TEXTURE);
	if(use_texture)
	{
		Shared::PBR_TEXTURE& texture = *material.pbr_mat.structure.texture;
		texture.is_smoothness = itr++->second.as<bool>();
		const bool combine_metallic_roughness = itr++->second.as<bool>();
		texture.use_combined_metallic_roughness = combine_metallic_roughness;
		texture.texture_names[Shared::ALBEDO] = itr++->second.as<std::string>();
		texture.texture_names[Shared::NORMAL] = itr++->second.as<std::string>();
		if(combine_metallic_roughness)
		{
			texture.texture_names[Shared::METALLIC_ROUGHNESS] = itr++->second.as<std::string>();
		}
		else
		{
			texture.texture_names[Shared::METALLIC] = itr++->second.as<std::string>();
			texture.texture_names[Shared::ROUGHNESS] = itr++->second.as<std::string>();
		}
		texture.texture_names[Shared::AO] = itr++->second.as<std::string>();
		texture.texture_names[Shared::EMISSIVE] = itr++->second.as<std::string>();
	}
	else
	{
		Shared::PBR_NONE_TEXTURE& non_texture = *material.pbr_mat.structure.non_texture;
		//node["albedo"];
		_deserialize(node["albedo"], non_texture.albedo, nullptr);
		//node["albedo"] >> non_texture.albedo;
		++itr;
		non_texture.metallic = itr++->second.as<float>();
		non_texture.roughness = itr++->second.as<float>();
		non_texture.ao = itr++->second.as<float>();
	}

	return node;
}
