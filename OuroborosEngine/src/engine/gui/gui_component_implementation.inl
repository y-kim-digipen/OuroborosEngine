namespace OE
{
	template<>
	inline void ComponentDrawFunction<MeshComponent>(ecs_ID entID)
	{
		static char* buffer = new char[30]();
		std::string strID = std::to_string(entID);
		MeshComponent& mesh_component = ecs_manager.GetComponent<MeshComponent>(entID);
		memcpy(buffer, mesh_component.mesh_name.c_str(), 30);
		if (ImGui::InputText("Meshname", buffer, 30, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			mesh_component.mesh_name = buffer;
		}
	}

	template<>
	inline void ComponentDrawFunction<TransformComponent>(ecs_ID entID)
	{
		std::string strID = std::to_string(entID);
		TransformComponent& transform_component = ecs_manager.GetComponent<TransformComponent>(entID);
		if (ImGui::TreeNode(typeid(TransformComponent).name()))
		{
			ImGui::DragFloat3(GET_VARIABLE_NAME(transform_component.pos), &transform_component.pos.x);
			//ImGui::DragFloat3(GET_VARIABLE_NAME(transform_component.scale), &transform_component.scale.x);
			//ImGui::DragFloat3(GET_VARIABLE_NAME(transform_component.rotateAxis), &transform_component.rotate_axis.x);
			//ImGui::DragFloat(GET_VARIABLE_NAME(transform_component.angle), &transform_component.angle);
			ImGui::TreePop();
		}
	}

	template<>
	inline void ComponentDrawFunction<VelocityComponent>(ecs_ID entID)
	{
		std::string strID = std::to_string(entID);
		VelocityComponent& velocity_component = ecs_manager.GetComponent<VelocityComponent>(entID);
		if (ImGui::TreeNode(typeid(VelocityComponent).name()))
		{
			ImGui::DragFloat3(GET_VARIABLE_NAME(velocity_component.vel), &velocity_component.vel.x);
			ImGui::TreePop();
		}
	}

	template<>
	inline void ComponentDrawFunction<LifeTimeComponent>(ecs_ID entID)
	{
		std::string strID = std::to_string(entID);
		LifeTimeComponent& life_time_component = ecs_manager.GetComponent<LifeTimeComponent>(entID);
		if (ImGui::TreeNode(typeid(LifeTimeComponent).name()))
		{
			ImGui::DragFloat(GET_VARIABLE_NAME(life_time_component.life_time), &life_time_component.life_time);
			ImGui::TreePop();
		}
	}

	template<>
	inline void ComponentDrawFunction<TagComponent>(ecs_ID entID)
	{
		std::string strID = std::to_string(entID);
		TagComponent& tag_component = ecs_manager.GetComponent<TagComponent>(entID);
		if (ImGui::TreeNode(typeid(TagComponent).name()))
		{
			constexpr int BufSize = 256;
			static char buf[BufSize];
			//std::vector<char> buf(tag_component.tag.length());
			// ReSharper disable once CppDeprecatedEntity
			strcpy_s(buf, tag_component.tag.c_str());

			if (ImGui::InputText(GET_VARIABLE_NAME(life_time_component.life_time), buf, BufSize))
			{
				tag_component.tag = buf;
			}

			ImGui::TreePop();
		}
	}

	//template<>
	//inline void ComponentDrawFunction<MeshComponent>(ecs_ID entID)
	//{
	//	static char buffer[30];
	//	std::string strID = std::to_string(entID);
	//	MeshComponent& mesh_component = ecs_manager.GetComponent<MeshComponent>(entID);
	//	memcpy(buffer, mesh_component.mesh_name.c_str(), 30);
	//	if (ImGui::InputText("Meshname", buffer, 30, ImGuiInputTextFlags_EnterReturnsTrue))
	//	{
	//		mesh_component.mesh_name = buffer;
	//	}
	//}

	template<>
	inline void ComponentDrawFunction<ShaderComponent>(ecs_ID entID)
	{
		static char buffer[30];
		std::string strID = std::to_string(entID);
		ShaderComponent& shader_component = ecs_manager.GetComponent<ShaderComponent>(entID);
		memcpy(buffer, shader_component.name.c_str(), 30);
		if (ImGui::InputText("Shadername", buffer, 30, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			shader_component.name = buffer;
		}
	}

	template<>
	inline void ComponentDrawFunction<MaterialComponent>(ecs_ID entID)
	{
		static char buffer[30];
		std::string strID = std::to_string(entID);
		MaterialComponent& material_component = ecs_manager.GetComponent<MaterialComponent>(entID);
		memcpy(buffer, material_component.name.c_str(), 30);
		if (ImGui::InputText("Materialname", buffer, 30, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			material_component.name = buffer;
		}
		if (ImGui::TreeNode(typeid(MaterialComponent).name()))
		{
			ImGui::DragFloat3(GET_VARIABLE_NAME(material.ambient), &material_component.ambient.x);
			ImGui::DragFloat3(GET_VARIABLE_NAME(material.diffuse), &material_component.diffuse.x);
			ImGui::DragFloat3(GET_VARIABLE_NAME(material.specular), &material_component.specular.x);
			ImGui::TreePop();
		}

	}

}