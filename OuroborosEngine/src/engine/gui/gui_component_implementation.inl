namespace OE
{
	template<>
	inline void ComponentDrawFunction<Mesh>(ecs_ID entID)
	{
		static char* buffer = new char[30]();
		std::string strID = std::to_string(entID);
		Mesh& mesh_component = ecs_manager.GetComponent<Mesh>(entID);
		memcpy(buffer, mesh_component.mesh_name.c_str(), 30);
		if (ImGui::InputText("Meshname", buffer, 30, ImGuiInputTextFlags_EnterReturnsTrue))
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
					//ImGuiDragDropCopy<std::string>(mesh_component.mesh_name);
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
	inline void ComponentDrawFunction<Transform>(ecs_ID entID)
	{
		std::string strID = std::to_string(entID);
		TransformComponent& transform_component = ecs_manager.GetComponent<TransformComponent>(entID);
		//if (ImGui::TreeNode(typeid(TransformComponent).name()))
		//{
			ImGui::DragFloat3(GET_VARIABLE_NAME(transform_component.pos), &transform_component.pos.x);
			ImGuiDragDropCopy(transform_component.pos, ImGuiDragDropFlags_SourceAllowNullID);

			ImGui::DragFloat3(GET_VARIABLE_NAME(transform_component.scale), &transform_component.scale.x);
			ImGuiDragDropCopy(transform_component.scale, ImGuiDragDropFlags_SourceAllowNullID);

			ImGui::DragFloat3(GET_VARIABLE_NAME(transform_component.rotateAxis), &transform_component.rotate_axis.x);
			ImGuiDragDropCopy(transform_component.rotate_axis, ImGuiDragDropFlags_SourceAllowNullID);
			
			//ImGui::DragFloat ("GET_VARIABLE_NAME(transform_component.angle)", &transform_component.angle.x);
			ImGui::DragScalarN(GET_VARIABLE_NAME(transform_component.angle), ImGuiDataType_Float, &transform_component.angle.x, 1);
			ImGuiDragDropCopy(transform_component.angle, ImGuiDragDropFlags_SourceAllowNullID);
	}

	template<>
	inline void ComponentDrawFunction<Velocity>(ecs_ID entID)
	{
		std::string strID = std::to_string(entID);
		Velocity& velocity_component = ecs_manager.GetComponent<Velocity>(entID);
		if (ImGui::TreeNode(typeid(Velocity).name()))
		{
			ImGui::DragFloat3(GET_VARIABLE_NAME(velocity_component.vel), &velocity_component.vel.x);
			ImGui::TreePop();
		}
		ImGuiDragDropCopy(velocity_component.vel);
	}

	template<>
	inline void ComponentDrawFunction<LifeTime>(ecs_ID entID)
	{
		std::string strID = std::to_string(entID);
		LifeTime& life_time_component = ecs_manager.GetComponent<LifeTime>(entID);
		if (ImGui::TreeNode(typeid(LifeTime).name()))
		{
			ImGui::DragFloat(GET_VARIABLE_NAME(life_time_component.life_time), &life_time_component.life_time);
			ImGuiDragDropCopy(life_time_component.life_time);
			ImGui::TreePop();
		}
	}

	template<>
	inline void ComponentDrawFunction<Tag>(ecs_ID entID)
	{
		std::string strID = std::to_string(entID);
		Tag& tag_component = ecs_manager.GetComponent<Tag>(entID);
		if (ImGui::TreeNode(typeid(Tag).name()))
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
}
