template<typename T,
	std::enable_if_t<std::is_same_v<T, TransformComponent>, void*> = nullptr>
YAML::Emitter& _serialize(YAML::Emitter& emitter, T& transform, void*)
{
	glm::vec3 pos = transform.GetPosition();
	glm::vec3 scale = transform.GetScale();
	glm::vec3 rotation = transform.GetRotation();
	emitter << YAML::Key << "position" << YAML::Value << YAML::Flow << YAML::BeginSeq << pos.x << pos.y << pos.z << YAML::EndSeq;
	emitter << YAML::Key << "scale" << YAML::Value << YAML::Flow << YAML::BeginSeq << scale.x << scale.y << scale.z << YAML::EndSeq;
	emitter << YAML::Key << "rotation" << YAML::Value << YAML::Flow << YAML::BeginSeq << rotation.x << rotation.y << rotation.z << YAML::EndSeq;
	return emitter;
}

template<typename T,
	std::enable_if_t<std::is_same_v<T, MeshComponent>, void*> = nullptr>
YAML::Emitter& _serialize(YAML::Emitter& emitter, T& mesh, void*)
{
	emitter << YAML::Key << "mesh_name" << YAML::Value << mesh.mesh_name;
	return emitter;
}

template<typename T,
	std::enable_if_t<std::is_same_v<T, MaterialComponent>, void*> = nullptr>
YAML::Emitter& _serialize(YAML::Emitter& emitter, T& material, void*)
{
	emitter << YAML::Key << "name" << YAML::Value << material.name;
	emitter << YAML::Key << "material_data" << YAML::Value << YAML::BeginMap << material.data << YAML::EndMap;
	emitter << YAML::Key << "flag" << YAML::Value << material.flag;
	emitter << YAML::Key << "is_save" << YAML::Value << material.is_save;
	emitter << YAML::Key << "is_light" << YAML::Value << material.is_light;
	emitter << YAML::Key << "texture_albedo_name" << YAML::Value << material.texture_albedo_name;
	emitter << YAML::Key << "texture_normal_name" << YAML::Value << material.texture_normal_name;
	emitter << YAML::Key << "texture_metalroughness_name" << YAML::Value << material.texture_metalroughness_name;
	emitter << YAML::Key << "texture_ao_name" << YAML::Value << material.texture_ao_name;
	emitter << YAML::Key << "texture_emissive_name" << YAML::Value << material.texture_emissive_name;
	emitter << YAML::Key << "texture_roughness_name" << YAML::Value << material.texture_roughness_name;
	return emitter;
}

template<typename T,
	std::enable_if_t<std::is_same_v<T, ShaderComponent>, void*> = nullptr>
YAML::Emitter& _serialize(YAML::Emitter& emitter, T& shader, void*)
{
	emitter << YAML::Key << "name" << YAML::Value << shader.name;
	return emitter;
}

template<typename T,
	std::enable_if_t<std::is_same_v<T, TagComponent>, void*> = nullptr>
YAML::Emitter& _serialize(YAML::Emitter& emitter, T& tag, void*)
{
	emitter << YAML::Key << "tag" << YAML::Value << tag.tag;
	return emitter;
}

template<typename T,
	std::enable_if_t<std::is_same_v<T, LightComponent>, void*> = nullptr>
YAML::Emitter& _serialize(YAML::Emitter& emitter, T& light, void*)
{
	emitter << YAML::Key << "init" << YAML::Value << false;
	emitter << YAML::Key << "data.position" << YAML::Value << light.data.pos;
	emitter << YAML::Key << "data.cutoff" << YAML::Value << light.data.cutoff;
	emitter << YAML::Key << "data.diffuse" << YAML::Value << light.data.diffuse;
	emitter << YAML::Key << "data.out_cutoff" << YAML::Value << light.data.out_cutoff;
	emitter << YAML::Key << "data.dir" << YAML::Value << light.data.dir;
	emitter << YAML::Key << "data.falloff" << YAML::Value << light.data.falloff;
	emitter << YAML::Key << "data.type" << YAML::Value << light.data.type;
	return emitter;
}

template<typename T,
	std::enable_if_t<std::is_same_v<T, ScriptComponent>, void*> = nullptr>
YAML::Emitter& _serialize(YAML::Emitter& emitter, T& script, void*)
{
	emitter << YAML::Key << "name" << YAML::Value << script.name;
	return emitter;
}