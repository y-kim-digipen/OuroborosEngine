template<typename T,
	std::enable_if_t<std::is_same_v<T, TransformComponent>, void*> = nullptr>
YAML::Node _deserialize(YAML::Node node, T& transform, void*)
{
	glm::vec3 pos, scale, rot;
	_deserialize(node["position"], pos, nullptr);
	_deserialize(node["scale"], scale, nullptr);
	_deserialize(node["rotation"], rot, nullptr);
	//node["position"] >> pos;
	//node["scale"] >> scale;
	//node["rotation"] >> rot;

	transform.SetPosition(pos);
	transform.SetScale(scale);
	transform.SetRotation(rot);
	return node;
}

template<typename T,
	std::enable_if_t<std::is_same_v<T, MeshComponent>, void*> = nullptr>
YAML::Node _deserialize(YAML::Node node, T& mesh, void*)
{
	node["mesh_name"] >> mesh.mesh_name;
	return node;
}

template<typename T,
	std::enable_if_t<std::is_same_v<T, MaterialComponent>, void*> = nullptr>
YAML::Node _deserialize(YAML::Node node, T& material, void*)
{
	node["name"] >> material.name;
	node["material_data"] >> material.data;
	node["flag"] >> material.flag;
	node["is_save"] >> material.is_save;
	node["is_light"] >> material.is_light;
	node["texture_albedo_name"] >> material.texture_albedo_name;
	node["texture_normal_name"] >> material.texture_normal_name;
	node["texture_metalroughness_name"] >> material.texture_metalroughness_name;
	node["texture_ao_name"] >> material.texture_ao_name;
	node["texture_emissive_name"] >> material.texture_emissive_name;
	node["texture_roughness_name"] >> material.texture_roughness_name;
	return node;
}

template<typename T,
	std::enable_if_t<std::is_same_v<T, ShaderComponent>, void*> = nullptr>
YAML::Node _deserialize(YAML::Node node, T& shader, void*)
{
	node["name"] >> shader.name;
	return node;
}

template<typename T,
	std::enable_if_t<std::is_same_v<T, TagComponent>, void*> = nullptr>
YAML::Node _deserialize(YAML::Node node, T& tag, void*)
{
	node["tag"] >> tag.tag;
	return node;
}

template<typename T,
	std::enable_if_t<std::is_same_v<T, LightComponent>, void*> = nullptr>
YAML::Node _deserialize(YAML::Node node, T& light, void*)
{
	node["init"] >> light.init;
	node["data.position"] >> light.data.pos;
	node["data.cutoff"] >> light.data.cutoff;
	node["data.diffuse"] >> light.data.diffuse;
	node["data.out_cutoff"] >> light.data.out_cutoff;
	node["data.dir"] >> light.data.dir;
	node["data.falloff"] >> light.data.falloff;
	node["data.type"] >> light.data.type;
	return node;
}

template<typename T,
	std::enable_if_t<std::is_same_v<T, ScriptComponent>, void*> = nullptr>
YAML::Node _deserialize(YAML::Node node, T& script, void*)
{
	node["name"] >> script.name;
	return node;
}
