namespace OE
{
	template<>
	inline void ComponentDrawFunction<MeshComponent>(ecs_ID entID)
	{
		static char buffer[30];
		std::string strID = std::to_string(entID);
		MeshComponent& mesh_component = ecs_manager.GetComponent<MeshComponent>(entID);
		memcpy(buffer, mesh_component.mesh_name.c_str(), 30);
		const auto& mesh_map = Engine::Get().asset_manager.GetManager<MeshAssetManager>().GetAssetRawData();
		if (ImGui::TreeNode(typeid(MeshComponent).name()))
		{
			if (ImGui::BeginCombo("Meshes", mesh_component.mesh_name.c_str()))
			{
				for (const auto& key : mesh_map | std::views::keys)
				{
					const bool selected = mesh_component.mesh_name == key;
					if (ImGui::Selectable(key.c_str(), selected))
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
		ImGuiDragDropCopy(mesh_component.mesh_name);
	}



	template<>
	inline void ComponentDrawFunction<TransformComponent>(ecs_ID entID)
	{
		std::string strID = std::to_string(entID);
		TransformComponent& transform_component = ecs_manager.GetComponent<TransformComponent>(entID);
		if (ImGui::TreeNode(typeid(TransformComponent).name()))
		{
			ImGui::DragFloat3(GET_VARIABLE_NAME(transform_component.pos), &transform_component.pos.x);
			ImGuiDragDropCopy(transform_component.pos, ImGuiDragDropFlags_SourceAllowNullID);

			ImGui::DragFloat3(GET_VARIABLE_NAME(transform_component.scale), &transform_component.scale.x);
			ImGuiDragDropCopy(transform_component.scale, ImGuiDragDropFlags_SourceAllowNullID);

			ImGui::DragFloat3(GET_VARIABLE_NAME(transform_component.rotateAxis), &transform_component.rotate_axis.x);
			ImGuiDragDropCopy(transform_component.rotate_axis, ImGuiDragDropFlags_SourceAllowNullID);

			//ImGui::DragFloat ("GET_VARIABLE_NAME(transform_component.angle)", &transform_component.angle.x);
			ImGui::DragScalarN(GET_VARIABLE_NAME(transform_component.angle), ImGuiDataType_Float, &transform_component.angle, 1);
			ImGuiDragDropCopy(transform_component.angle, ImGuiDragDropFlags_SourceAllowNullID);
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
		ImGuiDragDropCopy(velocity_component.vel, ImGuiDragDropFlags_SourceAllowNullID);
	}

	template<>
	inline void ComponentDrawFunction<LifeTimeComponent>(ecs_ID entID)
	{
		std::string strID = std::to_string(entID);
		LifeTimeComponent& life_time_component = ecs_manager.GetComponent<LifeTimeComponent>(entID);
		if (ImGui::TreeNode(typeid(LifeTimeComponent).name()))
		{
			ImGui::DragFloat(GET_VARIABLE_NAME(life_time_component.life_time), &life_time_component.life_time);
			ImGuiDragDropCopy(life_time_component.life_time);
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
		if (ImGui::TreeNode(typeid(ShaderComponent).name()))
		{
			memcpy(buffer, shader_component.name.c_str(), 30);
			if(ImGui::Button("light_shader"))
			{
				shader_component.name = "light_shader";
			}
			ImGui::SameLine();
			if(ImGui::Button("shader1"))
			{
				shader_component.name = "shader";
			}
			ImGui::SameLine();
			if(ImGui::Button("shader2"))
			{
				shader_component.name = "shader2";
			}

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
			memcpy(buffer, material_component.name.c_str(), 30);
			if (ImGui::InputText("Materialname", buffer, 30, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				material_component.name = buffer;
				material_component.flag = true;
			}
			bool is_changed = ImGui::ColorEdit3(GET_VARIABLE_NAME(material.ambient), &material_component.data.ambient.x);
			is_changed |= ImGui::ColorEdit3(GET_VARIABLE_NAME(material.diffuse), &material_component.data.diffuse.x);
			is_changed |= ImGui::ColorEdit3(GET_VARIABLE_NAME(material.specular), &material_component.data.specular.x);
			is_changed |= ImGui::DragFloat(GET_VARIABLE_NAME(material.shinnesss), &material_component.data.shininess);

			if (is_changed)
			{
				material_component.flag = true;
			}

			if (ImGui::Button("save"))
			{
				material_component.is_save = true;
			}

			ImGui::TreePop();
		}
	}


	template<>
	inline void ComponentDrawFunction<LightComponent>(ecs_ID entID)
	{
		std::string strID = std::to_string(entID);
		LightComponent& light_component = ecs_manager.GetComponent<LightComponent>(entID);
		if (ImGui::TreeNode(typeid(LightComponent).name()))
		{
			//ImGui::DragFloat3(GET_VARIABLE_NAME(light.pos), &light_component.data.position.x);
			ImGui::DragFloat3(GET_VARIABLE_NAME(light.direction), &light_component.data.direction.x);
			ImGui::ColorEdit3(GET_VARIABLE_NAME(light.diffuse), &light_component.data.diffuse.x);
			ImGui::ColorEdit3(GET_VARIABLE_NAME(light.ambient), &light_component.data.ambient.x);
			ImGui::ColorEdit3(GET_VARIABLE_NAME(light.specular), &light_component.data.specular.x);
			ImGui::TreePop();
		}

	}
}