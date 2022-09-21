template<typename T,
	std::enable_if_t<std::is_same_v<T, Asset::MaterialData>, void*> = nullptr>
YAML::Node _deserialize(YAML::Node node, T& material_data, void*)
{
	node["albedo"] >> material_data.albedo;
	node["metallic"] >> material_data.metallic;
	node["roughness"] >> material_data.roughness;
	node["ao"] >> material_data.ao;
	node["has_albedo_texture"] >> material_data.has_albedo_texture;
	node["has_normal_texture"] >> material_data.has_normal_texture;
	node["has_metalroughness_texture"] >> material_data.has_metalroughness_texture;
	node["has_ao_texture"] >> material_data.has_ao_texture;
	node["has_metalic_texture"] >> material_data.has_metalic_texture;
	node["has_roughness_texture"] >> material_data.has_roughness_texture;
	node["has_emissive_texture"] >> material_data.has_emissive_texture;
	node["is_roughness_texture_inverted"] >> material_data.is_roughness_texture_inverted;
	return node;
}
