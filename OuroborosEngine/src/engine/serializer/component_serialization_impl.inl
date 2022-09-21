template<typename T,
	std::enable_if_t<std::is_same_v<T, TransformComponent>, void*> = nullptr>
YAML::Emitter& _serialize(YAML::Emitter& emitter, T& transform, void*)
{
	emitter << YAML::Key << "pos" << YAML::Value << transform.pos;
	emitter << YAML::Key << "scale" << YAML::Value << transform.scale;
	emitter << YAML::Key << "rotation" << YAML::Value << transform.rotation;
	return emitter;
}

template<typename T,
	std::enable_if_t<std::is_same_v<T, VelocityComponent>, void*> = nullptr>
YAML::Emitter& _serialize(YAML::Emitter& emitter, T& velocity, void*)
{
	emitter << YAML::Key << "vel" << YAML::Value << velocity.vel;
	return emitter;
}

template<typename T,
	std::enable_if_t<std::is_same_v<T, LifeTimeComponent>, void*> = nullptr>
YAML::Emitter& _serialize(YAML::Emitter& emitter, T& lifetime, void*)
{
	emitter << YAML::Key << "life_time" << YAML::Value << lifetime.life_time;
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
	std::enable_if_t<std::is_same_v<T, BoolWrapperComponent>, void*> = nullptr>
YAML::Emitter& _serialize(YAML::Emitter& emitter, T& bool_wrapper, void*)
{
	emitter << YAML::Key << "bool_type" << YAML::Value << bool_wrapper.bool_type;
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
	emitter << YAML::Key << "data.pos" << YAML::Value << light.data.pos;
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