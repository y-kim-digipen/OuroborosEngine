namespace OE
{
	template<>
	inline void ComponentDrawFunction<MeshComponent>(ecs_ID entID)
	{
		static char* buffer = new char[30]();
		std::string strID = std::to_string(entID);
		MeshComponent& mesh_component = ecs_manager.GetComponent<MeshComponent>(entID);
		memcpy(buffer, mesh_component.mesh_name.c_str(), 30);
		const auto& mesh_map = Engine::Get().asset_manager.GetManager<MeshAssetManager>().GetAssetRawData();
		if (ImGui::TreeNode(typeid(MeshComponent).name()))
		{
			if(ImGui::BeginCombo("Meshes", mesh_component.mesh_name.c_str()))
			{
				for (const auto& key : mesh_map | std::views::keys)
				{
					const bool selected = mesh_component.mesh_name == key;
					if(ImGui::Selectable(key.c_str(), selected))
					{
						mesh_component.mesh_name = key;
					}
					if (selected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			ImGui::TreePop();
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
			ImGui::DragFloat3(GET_VARIABLE_NAME(transform_component.scale), &transform_component.scale.x);
			ImGui::DragFloat3(GET_VARIABLE_NAME(transform_component.rotateAxis), &transform_component.rotate_axis.x);
			float angle = glm::degrees(transform_component.angle);

			if(ImGui::DragFloat(GET_VARIABLE_NAME(transform_component.angle), &angle))
			{
				transform_component.angle = glm::radians(angle);
			};

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

	template<>
	inline void ComponentDrawFunction<ShaderComponent>(ecs_ID entID)
	{
		static char buffer[30];
		std::string strID = std::to_string(entID);
		ShaderComponent& shader_component = ecs_manager.GetComponent<ShaderComponent>(entID);
		memcpy(buffer, shader_component.name.c_str(), 30);
		if (ImGui::TreeNode(typeid(ShaderComponent).name()))
		{
			if (ImGui::InputText("Shadername", buffer, 30, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				shader_component.name = buffer;
			}
			ImGui::TreePop();
		}
	}

	template<>
	inline void ComponentDrawFunction<MaterialComponent>(ecs_ID entID)
	{
		static char buffer[30];
		std::string strID = std::to_string(entID);
		MaterialComponent& material_component = ecs_manager.GetComponent<MaterialComponent>(entID);
		memcpy(buffer, material_component.name.c_str(), 30);
		if (ImGui::TreeNode(typeid(MaterialComponent).name()))
		{
		if (ImGui::InputText("Materialname", buffer, 30, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			material_component.name = buffer;
		}
			ImGui::DragFloat3(GET_VARIABLE_NAME(material.ambient), &material_component.ambient.x);
			ImGui::DragFloat3(GET_VARIABLE_NAME(material.diffuse), &material_component.diffuse.x);
			ImGui::DragFloat3(GET_VARIABLE_NAME(material.specular), &material_component.specular.x);
			ImGui::TreePop();
		}

	}

}
