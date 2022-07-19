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
			ImGui::DragFloat3(GET_VARIABLE_NAME(transform_component.pos), &transform_component.pos.x,0.1);
			ImGuiDragDropCopy(transform_component.pos, ImGuiDragDropFlags_SourceAllowNullID);

			ImGui::DragFloat3(GET_VARIABLE_NAME(transform_component.scale), &transform_component.scale.x, 0.1);
			ImGuiDragDropCopy(transform_component.scale, ImGuiDragDropFlags_SourceAllowNullID);

			ImGui::DragFloat3(GET_VARIABLE_NAME(transform_component.rotateAxis), &transform_component.rotate_axis.x, 0.1);
			ImGuiDragDropCopy(transform_component.rotate_axis, ImGuiDragDropFlags_SourceAllowNullID);

			//ImGui::DragFloat ("GET_VARIABLE_NAME(transform_component.angle)", &transform_component.angle.x);

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
				if(shader->uniform_buffer_object != nullptr)
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
		MaterialComponent& material_component = ecs_manager.GetComponent<MaterialComponent>(entID);
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

			bool is_changed = ImGui::ColorEdit3(GET_VARIABLE_NAME(material.albedo), &material_component.data.albedo.x);
			is_changed |= ImGui::DragFloat(GET_VARIABLE_NAME(material.metallic), &material_component.data.metallic, 0.01f, 0.0f, 1.0f);
			is_changed |= ImGui::DragFloat(GET_VARIABLE_NAME(material.roughness), &material_component.data.roughness, 0.01f, 0.0f, 1.0f);
			is_changed |= ImGui::DragFloat(GET_VARIABLE_NAME(material.ao), &material_component.data.ao, 0.1f);



			if (ImGui::Button("save"))
			{
				material_component.is_save = true;
			}

			const auto& texture_manager = Engine::Get().window->GetWindowData().RenderContextData->texture_manager_;


			is_changed |= ImGui::Checkbox("AlbedoTexture_check", reinterpret_cast<bool*>(&material_component.data.has_albedo_texture));

			if (material_component.data.has_albedo_texture)
			{
				if (ImGui::BeginCombo("AlbedoTexture", material_component.texture_albedo_name.c_str()))
				{
					for (const auto& key : texture_map | std::views::keys)
					{
						const bool selected = material_component.texture_albedo_name == key;
						if (ImGui::Selectable(key.c_str(), selected))
						{
							material_component.texture_albedo_name = key;
							is_changed |= true;

						}
						if (selected) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
				if (auto texture = texture_manager->GetTexture(material_component.texture_albedo_name); texture)
				{
					dynamic_cast<Renderer::VulkanTexture*>(texture.get())->UpdateToDescripterSet(dynamic_cast<Renderer::VulkanTextureManager*>(texture_manager.get())->descriptor_set_[0], 0);
					ImGui::Image(dynamic_cast<Renderer::VulkanTextureManager*>(texture_manager.get())->descriptor_set_[0], ImVec2(100, 100));
				}

			}
			else
			{
				material_component.texture_albedo_name = "";
			}

		
			


			is_changed |= ImGui::Checkbox("NormalTexture_check", reinterpret_cast<bool*>(&material_component.data.has_normal_texture));
			if (material_component.data.has_normal_texture)
			{
				if (ImGui::BeginCombo("NormalTexture", material_component.texture_normal_name.c_str()))
				{
					for (const auto& key : texture_map | std::views::keys)
					{
						const bool selected = material_component.texture_normal_name == key;
						if (ImGui::Selectable(key.c_str(), selected))
						{
							material_component.texture_normal_name = key;
							is_changed |= true;
						}
						if (selected) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
				if (auto texture = texture_manager->GetTexture(material_component.texture_normal_name); texture)
				{
					dynamic_cast<Renderer::VulkanTexture*>(texture.get())->UpdateToDescripterSet(dynamic_cast<Renderer::VulkanTextureManager*>(texture_manager.get())->descriptor_set_[1], 0);
					ImGui::Image(dynamic_cast<Renderer::VulkanTextureManager*>(texture_manager.get())->descriptor_set_[1], ImVec2(100, 100));
				}
			}
			else
			{
				material_component.texture_normal_name = "";
			}
			


			is_changed |= ImGui::Checkbox("MetalRoughnessTexture_check", reinterpret_cast<bool*>(&material_component.data.has_metalroughness_texture));
			if (material_component.data.has_metalroughness_texture)
			{
				if (ImGui::BeginCombo("MetalRoughnessTexture", material_component.texture_metalroughness_name.c_str()))
				{
					for (const auto& key : texture_map | std::views::keys)
					{
						const bool selected = material_component.texture_metalroughness_name == key;
						if (ImGui::Selectable(key.c_str(), selected))
						{
							material_component.texture_metalroughness_name = key;
							is_changed |= true;
						}
						if (selected) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}

				if (auto texture = texture_manager->GetTexture(material_component.texture_metalroughness_name); texture)
				{
					dynamic_cast<Renderer::VulkanTexture*>(texture.get())->UpdateToDescripterSet(dynamic_cast<Renderer::VulkanTextureManager*>(texture_manager.get())->descriptor_set_[2], 0);
					ImGui::Image(dynamic_cast<Renderer::VulkanTextureManager*>(texture_manager.get())->descriptor_set_[2], ImVec2(100, 100));
				}

			}
			else
			{
				material_component.texture_metalroughness_name = "";
			}
			


			is_changed |= ImGui::Checkbox("AOTexture_check", reinterpret_cast<bool*>(&material_component.data.has_ao_texture));
			if(material_component.data.has_ao_texture)
			{
				
				if (ImGui::BeginCombo("AOTexture", material_component.texture_ao_name.c_str()))
				{
					for (const auto& key : texture_map | std::views::keys)
					{
						const bool selected = material_component.texture_ao_name == key;
						if (ImGui::Selectable(key.c_str(), selected))
						{
						material_component.texture_ao_name = key;
						is_changed |= true;
						}
						if (selected) {
						ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
				if (auto texture = texture_manager->GetTexture(material_component.texture_ao_name); texture)
				{
					dynamic_cast<Renderer::VulkanTexture*>(texture.get())->UpdateToDescripterSet(dynamic_cast<Renderer::VulkanTextureManager*>(texture_manager.get())->descriptor_set_[3], 0);
					ImGui::Image(dynamic_cast<Renderer::VulkanTextureManager*>(texture_manager.get())->descriptor_set_[3], ImVec2(100, 100));
				}
			}
			else
			{
				material_component.texture_ao_name = "";
			}


			

			if (is_changed)
			{
				material_component.flag = true;
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
			ImGui::DragFloat3(GET_VARIABLE_NAME(light.direction), &light_component.data.dir.x);
			ImGui::ColorEdit3(GET_VARIABLE_NAME(light.diffuse), &light_component.data.diffuse.x);
			ImGui::DragFloat(GET_VARIABLE_NAME(light.inner_cut_off), &light_component.data.cutoff);
			ImGui::DragFloat(GET_VARIABLE_NAME(light.outer_cut_off), &light_component.data.out_cutoff);
			ImGui::DragFloat(GET_VARIABLE_NAME(light.falloff), &light_component.data.falloff);
			ImGui::InputInt(GET_VARIABLE_NAME(light.type), &light_component.data.type);
			ImGui::TreePop();
		}

	}
}