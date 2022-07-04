namespace OE
{
	template<>
	inline void ComponentDrawFunction<MeshComponent>(ecs_ID entID)
	{
		static char buffer[30];
		std::string strID = std::to_string(entID);
		MeshComponent& mesh_component = OE::Engine::ecs_manager.GetComponent<MeshComponent>(entID);
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
		TransformComponent& transform_component = OE::Engine::ecs_manager.GetComponent<TransformComponent>(entID);
		if (ImGui::TreeNode(typeid(TransformComponent).name()))
		{
			ImGui::DragFloat3(GET_VARIABLE_NAME(transform_component.pos), &transform_component.pos.x);
			ImGuiDragDropCopy(transform_component.pos, ImGuiDragDropFlags_SourceAllowNullID);

			ImGui::DragFloat3(GET_VARIABLE_NAME(transform_component.scale), &transform_component.scale.x);
			ImGuiDragDropCopy(transform_component.scale, ImGuiDragDropFlags_SourceAllowNullID);

			ImGui::DragFloat3(GET_VARIABLE_NAME(transform_component.rotation), &transform_component.rotation.x);
			ImGuiDragDropCopy(transform_component.rotation, ImGuiDragDropFlags_SourceAllowNullID);

			//ImGui::DragFloat ("GET_VARIABLE_NAME(transform_component.angle)", &transform_component.angle.x);

			ImGui::TreePop();
		}
	}

	template<>
	inline void ComponentDrawFunction<VelocityComponent>(ecs_ID entID)
	{
		std::string strID = std::to_string(entID);
		VelocityComponent& velocity_component = OE::Engine::ecs_manager.GetComponent<VelocityComponent>(entID);
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
		LifeTimeComponent& life_time_component = OE::Engine::ecs_manager.GetComponent<LifeTimeComponent>(entID);
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
		TagComponent& tag_component = OE::Engine::ecs_manager.GetComponent<TagComponent>(entID);
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
		ShaderComponent& shader_component = Engine::ecs_manager.GetComponent<ShaderComponent>(entID);
		const auto& shader = Engine::window->GetWindowData().RenderContextData->shader_manager->GetShader(shader_component.name);
		const auto& shader_map = Engine::Get().asset_manager.GetManager<ShaderAssetManager>().GetAssetRawData();

		if (ImGui::TreeNode(typeid(ShaderComponent).name()))
		{
			if (ImGui::BeginCombo("Shader", shader_component.name.c_str()))
			{
				for (const auto& key : shader_map | std::views::keys)
				{
					const bool selected = shader_component.name == key;
					if (ImGui::Selectable(key.c_str(), selected))
					{
						shader_component.name = key;
					}
					if (selected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			if (shader_map.find(shader_component.name) != shader_map.end()) {
				
				for (const auto& member_variable : shader->uniform_buffer_object->member_vars) {

					switch (member_variable.second.type) {
					case Renderer::DataType::BOOL:
						ImGui::Checkbox(member_variable.first.c_str(), (bool*)shader->GetMemberVariable(member_variable.first));
						break;
					case Renderer::DataType::FLOAT:
						ImGui::DragFloat(member_variable.first.c_str(), (float*)shader->GetMemberVariable(member_variable.first));
						break;
					case Renderer::DataType::FLOAT2:
						ImGui::DragFloat2(member_variable.first.c_str(), (float*)shader->GetMemberVariable(member_variable.first));
						break;
					case Renderer::DataType::FLOAT3:
						ImGui::DragFloat3(member_variable.first.c_str(), (float*)shader->GetMemberVariable(member_variable.first));
						break;
					case Renderer::DataType::FLOAT4:
						ImGui::DragFloat4(member_variable.first.c_str(), (float*)shader->GetMemberVariable(member_variable.first));
						break;
					case Renderer::DataType::INT:
						ImGui::DragInt(member_variable.first.c_str(), (int*)shader->GetMemberVariable(member_variable.first));
						break;
					case Renderer::DataType::INT2:
						ImGui::DragInt2(member_variable.first.c_str(), (int*)shader->GetMemberVariable(member_variable.first));
						break;
					case Renderer::DataType::INT3:
						ImGui::DragInt3(member_variable.first.c_str(), (int*)shader->GetMemberVariable(member_variable.first));
						break;
					case Renderer::DataType::INT4:
						ImGui::DragInt4(member_variable.first.c_str(), (int*)shader->GetMemberVariable(member_variable.first));
						break;
					case Renderer::DataType::MAT3:
						break;
					case Renderer::DataType::MAT4:
						break;
					}

				}
			}

			ImGui::TreePop();
		}

	}



	template<>
	inline void ComponentDrawFunction<MaterialComponent>(ecs_ID entID)
	{
		static char buffer[30];
		std::string strID = std::to_string(entID);
		MaterialComponent& material_component = OE::Engine::ecs_manager.GetComponent<MaterialComponent>(entID);
		memcpy(buffer, material_component.name.c_str(), 30);
		const auto& texture_map = Engine::Get().asset_manager.GetManager<ImageAssetManager>().GetAssetRawData();
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


		
			if (ImGui::BeginCombo("Texture", material_component.texture_name.c_str()))
			{
				for (const auto& key : texture_map | std::views::keys)
				{
					const bool selected = material_component.texture_name == key;
					if (ImGui::Selectable(key.c_str(), selected))
					{
						material_component.texture_name = key;

					
					}
					if (selected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			const auto& texture_manager = Engine::Get().window->GetWindowData().RenderContextData->texture_manager_;
			if(auto texture = texture_manager->GetTexture(material_component.texture_name); texture)
			{
				dynamic_cast<Renderer::VulkanTexture*>(texture.get())->UpdateToDescripterSet(dynamic_cast<Renderer::VulkanTextureManager*>(texture_manager.get())->descriptor_set_, 0);
				ImGui::Image(dynamic_cast<Renderer::VulkanTextureManager*>(texture_manager.get())->descriptor_set_, ImVec2(300, 300));
			}
			
			ImGui::TreePop();
		}
	}


	template<>
	inline void ComponentDrawFunction<LightComponent>(ecs_ID entID)
	{
		std::string strID = std::to_string(entID);
		LightComponent& light_component = OE::Engine::ecs_manager.GetComponent<LightComponent>(entID);
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

	template<>
	inline void ComponentDrawFunction<BoolWrapperComponent>(ecs_ID entID)
	{
		std::string strID = std::to_string(entID);
		BoolWrapperComponent& bool_wrapper_component = OE::Engine::ecs_manager.GetComponent<BoolWrapperComponent>(entID);
		if (ImGui::TreeNode(typeid(BoolWrapperComponent).name()))
		{
			//ImGui::DragFloat3(GET_VARIABLE_NAME(light.pos), &light_component.data.position.x);
			ImGui::Checkbox(GET_VARIABLE_NAME(BoolWrapperComponent::bool_type), &bool_wrapper_component.bool_type);

			ImGui::TreePop();
		}

	}

	template<>
	inline void ComponentDrawFunction<ScriptComponent>(ecs_ID entID)
	{
		std::string strID = std::to_string(entID);
		ScriptComponent &script_component = OE::Engine::ecs_manager.GetComponent<ScriptComponent>(entID);
		using Script = OE::Script::Script;
		using ScriptType = OE::Script::ScriptType;
		Script* script = Engine::lua_script_manager.GetScript(ScriptType::Component, script_component.name);
		const std::string& using_script_path = script!= nullptr ? script->GetUsingScriptPath() : "";
		if (ImGui::TreeNode(typeid(ScriptComponent).name()))
		{
			if(ImGui::BeginCombo("ScriptPath", using_script_path.c_str()))
			{
				const auto& script_assets = Engine::asset_manager.GetManager<ScriptAssetManager>().GetAssetRawData();
				for (const auto& [string, script_asset] : script_assets)
				{
					const std::string& extension_str = std::filesystem::path(string).extension().string();
					if(extension_str == ".clua")
					{
						bool selected = string == using_script_path;
						if(ImGui::Selectable(string.c_str(), selected, selected || script_asset.first ? ImGuiSelectableFlags_Disabled : ImGuiSelectableFlags_None))
						{
							script_component.name = string;
							script->ChangeScript(string);
							/*if(pScript)
							{
								
							}*/
						}
					}
					if (ImGui::Selectable("##None", using_script_path.empty(), using_script_path.empty() ? ImGuiSelectableFlags_Disabled : ImGuiSelectableFlags_None))
					{
						script_component.name = "";
						script->ChangeScript("");
						/*if(pScript)
						{

						}*/
					}
				}
				ImGui::EndCombo();
			}


		/*	if(ImGui::SmallButton("Reload"))
			{
				script->ChangeScript(script_path);
			}*/
			//ImGui::DragFloat3(GET_VARIABLE_NAME(light.pos), &light_component.data.position.x);
			//ImGui::DragFloat3(GET_VARIABLE_NAME(light.direction), &light_component.data.direction.x);
			ImGui::TreePop();
		}

	}
}