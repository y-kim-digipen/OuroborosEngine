#include "GUIWrappedFunctions.h"

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
		GUI_Input::ImGuiDragDropCopy(mesh_component.mesh_name);
	}



	template<>
	inline void ComponentDrawFunction<TransformComponent>(ecs_ID entID)
	{
		std::string strID = std::to_string(entID);
		TransformComponent& transform_component = OE::Engine::ecs_manager.GetComponent<TransformComponent>(entID);
		if (ImGui::TreeNode(typeid(TransformComponent).name()))
		{
			GUI_Input::DragFloat3("Pos", &transform_component.pos.x);
			GUI_Input::ImGuiDragDropCopy(transform_component.pos, ImGuiDragDropFlags_SourceAllowNullID);

			GUI_Input::DragFloat3("Scale", &transform_component.scale.x);
			GUI_Input::ImGuiDragDropCopy(transform_component.scale, ImGuiDragDropFlags_SourceAllowNullID);

			GUI_Input::DragFloat3("Rot", &transform_component.rotation.x);
			GUI_Input::ImGuiDragDropCopy(transform_component.rotation, ImGuiDragDropFlags_SourceAllowNullID);

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
			GUI_Input::DragFloat3("Vel", &velocity_component.vel.x);
			ImGui::TreePop();
		}
		GUI_Input::ImGuiDragDropCopy(velocity_component.vel, ImGuiDragDropFlags_SourceAllowNullID);
	}

	template<>
	inline void ComponentDrawFunction<LifeTimeComponent>(ecs_ID entID)
	{
		std::string strID = std::to_string(entID);
		LifeTimeComponent& life_time_component = OE::Engine::ecs_manager.GetComponent<LifeTimeComponent>(entID);
		if (ImGui::TreeNode(typeid(LifeTimeComponent).name()))
		{
			GUI_Input::DragFloat("LifeTime", &life_time_component.life_time);
			GUI_Input::ImGuiDragDropCopy(life_time_component.life_time);
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

			if (shader_map.contains(shader_component.name)) {

				if (ImGui::Button("Reload"))
				{
					shader->reload_next_frame = true;
				}

				if(shader->uniform_buffer_object != nullptr)
				for (const auto& member_variable : shader->uniform_buffer_object->member_vars) {

					switch (member_variable.second.type) {
					case Renderer::DataType::BOOL:
						ImGui::Checkbox(member_variable.first.c_str(), (bool*)shader->GetMemberVariable(member_variable.first));
						break;
					case Renderer::DataType::FLOAT:
						ImGui::DragFloat(member_variable.first.c_str(), (float*)shader->GetMemberVariable(member_variable.first), Engine::window->vulkan_imgui_manager.GetSliderSpeed());
						break;
					case Renderer::DataType::FLOAT2:
						ImGui::DragFloat2(member_variable.first.c_str(), (float*)shader->GetMemberVariable(member_variable.first), Engine::window->vulkan_imgui_manager.GetSliderSpeed());
						break;
					case Renderer::DataType::FLOAT3:
						ImGui::DragFloat3(member_variable.first.c_str(), (float*)shader->GetMemberVariable(member_variable.first), Engine::window->vulkan_imgui_manager.GetSliderSpeed());
						break;
					case Renderer::DataType::FLOAT4:
						ImGui::DragFloat4(member_variable.first.c_str(), (float*)shader->GetMemberVariable(member_variable.first), Engine::window->vulkan_imgui_manager.GetSliderSpeed());
						break;
					case Renderer::DataType::INT:
						ImGui::DragInt(member_variable.first.c_str(), (int*)shader->GetMemberVariable(member_variable.first), Engine::window->vulkan_imgui_manager.GetSliderSpeed());
						break;
					case Renderer::DataType::INT2:
						ImGui::DragInt2(member_variable.first.c_str(), (int*)shader->GetMemberVariable(member_variable.first), Engine::window->vulkan_imgui_manager.GetSliderSpeed());
						break;
					case Renderer::DataType::INT3:
						ImGui::DragInt3(member_variable.first.c_str(), (int*)shader->GetMemberVariable(member_variable.first), Engine::window->vulkan_imgui_manager.GetSliderSpeed());
						break;
					case Renderer::DataType::INT4:
						ImGui::DragInt4(member_variable.first.c_str(), (int*)shader->GetMemberVariable(member_variable.first), Engine::window->vulkan_imgui_manager.GetSliderSpeed());
						break;
					case Renderer::DataType::MAT3:
						break;
					case Renderer::DataType::MAT4:
						break;
					//case Renderer::DataType::SAMPLER2D:
						//ImGui::
						//break;
					}
					shader->SetUniformValue(member_variable.first.c_str(), shader->GetMemberVariable(member_variable.first));

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

			const auto& material_datum = Engine::window->GetWindowData().RenderContextData->material_manager->GetMaterialDatum();
			static bool open = true;
			if(ImGui::BeginCombo("Material", material_component.name.c_str()))
			{
				for (const auto& material : material_datum)
				{
					const char* material_name = material.first.c_str();
					if(ImGui::Selectable(material_name, material_name == material_component.name))
					{
						
					}
				}

				if (ImGui::Selectable("New"))
				{
					open = true;
				}
				ImGui::EndCombo();
			}
			//Make new material
			static char material_name_buf[30];
			if (open)
			{
				ImGui::Begin("Make new material", &open);

				//Variables
				static bool using_texture = false;
				static float metallic = 0.0, roughness = 0.0, ao = 0.0;
				static glm::vec3 albedo(0.0);

				static std::string albedo_texture_name;
				static std::string normal_texture_name;
				static std::string metallic_roughness_texture_name;
				static std::string metallic_texture_name;
				static std::string roughness_texture_name;
				static std::string ao_texture_name;
				static std::string emissive_texture_name;

				static bool use_metallic_roughness = false;
				static bool invert_roughness = false;

				const bool has_same_name = material_datum.contains(material_name_buf);
				ImGui::InputTextWithHint("##name", "name", material_name_buf, 30);
				ImGui::SameLine();
				if (has_same_name)
				{
					ImGui::TextColored(ImVec4(0.6f, 0.f, 0.f, 1.f), "Already exist", material_name_buf);
				}
				else
				{
					if (ImGui::Button("Save"))
					{
						{
							material_component.name = material_name_buf;
							material_component.texture_albedo_name.clear();
							material_component.texture_normal_name.clear();
							material_component.texture_metalroughness_name.clear();
							material_component.texture_roughness_name.clear();
							material_component.texture_ao_name.clear();
							material_component.texture_emissive_name.clear();
						}


						auto& material_manager = Engine::window->GetWindowData().RenderContextData->material_manager;
						Asset::MaterialData material_data;
						if(using_texture == false)
						{
							material_data.albedo = albedo;
							material_data.metallic = metallic;
							material_data.roughness = roughness;
							material_data.ao = ao;
						}
						else
						{
							material_data.has_albedo_texture = !albedo_texture_name.empty();
							material_data.has_normal_texture = !normal_texture_name.empty();
							material_data.has_metalroughness_texture = !metallic_roughness_texture_name.empty();
							material_data.has_metalic_texture = !metallic_texture_name.empty();
							material_data.has_roughness_texture = !roughness_texture_name.empty();
							material_data.has_ao_texture = !ao_texture_name.empty();
							material_data.has_emissive_texture = !emissive_texture_name.empty();
							material_data.is_roughness_texture_inverted = invert_roughness;
						}

						material_component.data = material_data;

						material_manager->AddMaterial(material_name_buf, material_data);
						if(using_texture)
						{
							auto& texture_manager = Engine::Get().window->GetWindowData().RenderContextData->texture_manager_;
							auto material = material_manager->GetMaterial(material_name_buf);
							if(material_data.has_albedo_texture)
							{
								material_component.texture_albedo_name = albedo_texture_name;
								material->SetAlbedoTexture(texture_manager->GetTexture(albedo_texture_name));
							}
							if (material_data.has_normal_texture)
							{
								material_component.texture_normal_name = normal_texture_name;
								material->SetNormalTexture(texture_manager->GetTexture(normal_texture_name));
							}
							if (material_data.has_metalroughness_texture)
							{
								material_component.texture_metalroughness_name = metallic_roughness_texture_name;
								material->SetMetalRoughnessTexture(texture_manager->GetTexture(metallic_roughness_texture_name));
							}
							if (material_data.has_metalic_texture)
							{
								material_component.texture_metalroughness_name = metallic_texture_name;
								material->SetMetalicTexture(texture_manager->GetTexture(metallic_texture_name));
							}
							if (material_data.has_roughness_texture)
							{
								material_component.texture_roughness_name = roughness_texture_name;
								material->SetRoughSmoothnessTexture(texture_manager->GetTexture(roughness_texture_name));
							}
							if (material_data.has_ao_texture)
							{
								material_component.texture_ao_name = ao_texture_name;
								material->SetAOTexture(texture_manager->GetTexture(ao_texture_name));
							}
							if (material_data.has_emissive_texture)
							{
								material_component.texture_emissive_name = emissive_texture_name;
								material->SetEmissiveTexture(texture_manager->GetTexture(emissive_texture_name));
							}
						}

						//Cleanup
						{
							using_texture = false;
							metallic = 0.0, roughness = 0.0, ao = 0.0;
							albedo = glm::vec3(0.0);
							albedo_texture_name.clear();
							normal_texture_name.clear();
							metallic_roughness_texture_name.clear();
							metallic_texture_name.clear();
							roughness_texture_name.clear();
							ao_texture_name.clear();
							emissive_texture_name.clear();
							use_metallic_roughness = false;
							invert_roughness = false;
						}
					}
				}


				ImGui::Checkbox("Using Texture", &using_texture);
				ImGui::Separator();
				if(using_texture)
				{
					if(ImGui::Checkbox("combine metallic roughness", &use_metallic_roughness))
					{
						if (use_metallic_roughness)
						{
							metallic_texture_name.clear();
							roughness_texture_name.clear();
						}
						else
						{
							metallic_roughness_texture_name.clear();
						}
					}
					ImGui::Separator();
					OE::GUI_Input::TextureSelectable("albedo", albedo_texture_name);
					ImGui::Separator();
					OE::GUI_Input::TextureSelectable("normal", normal_texture_name);
					ImGui::Separator();

					if(use_metallic_roughness)
					{
						OE::GUI_Input::TextureSelectable("metallic / roughness", metallic_roughness_texture_name);
						ImGui::Separator();
					}
					else
					{
						OE::GUI_Input::TextureSelectable("metallic", metallic_texture_name);
						ImGui::Separator();
						OE::GUI_Input::TextureSelectable("roughness", roughness_texture_name);
						ImGui::Checkbox("Invert", &invert_roughness);
						ImGui::Separator();
					}

					OE::GUI_Input::TextureSelectable("ao", ao_texture_name);
					ImGui::Separator();
					OE::GUI_Input::TextureSelectable("emissive", emissive_texture_name);
				}
				else
				{
					ImGui::ColorEdit3("albedo", &albedo.x);
					GUI_Input::DragFloat("metallic", &metallic, 0.0, 1.0);
					GUI_Input::DragFloat("roughness", &roughness, 0.0f, 1.0f);
					GUI_Input::DragFloat("ao", &ao, 0.0, 1.0);
				}

				ImGui::End();
			}
			else
			{
				memset(buffer, 0, 30);
			}


			bool is_changed = ImGui::ColorEdit3(GET_VARIABLE_NAME(material.albedo), &material_component.data.albedo.x);

			is_changed |=  GUI_Input::DragFloat3("metallic", &material_component.data.metallic, 0.0, 1.0);
			is_changed |=  GUI_Input::DragFloat3("roughness", &material_component.data.roughness, 0.0f, 1.0f);
			is_changed |=  GUI_Input::DragFloat3("ao", &material_component.data.ao, 0.0, 1.0);

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
					const auto* TextureID = dynamic_cast<Renderer::VulkanTextureManager*>(texture_manager.get())->vulkan_texture_imgui_descriptor_pool.GetImGuiTextureID();
					dynamic_cast<Renderer::VulkanTexture*>(texture.get())->UpdateToDescripterSet(*TextureID, 0);
					ImGui::Image(*TextureID, ImVec2(100, 100));
				}
			}
			else
			{
				material_component.texture_albedo_name = "";
			}
			//ImGui::EndMEnu
			
			

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
					const auto* TextureID = dynamic_cast<Renderer::VulkanTextureManager*>(texture_manager.get())->vulkan_texture_imgui_descriptor_pool.GetImGuiTextureID();
					dynamic_cast<Renderer::VulkanTexture*>(texture.get())->UpdateToDescripterSet(*TextureID, 0);
					ImGui::Image(*TextureID, ImVec2(100, 100));
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
				ImGui::Checkbox("is_smoothness ", reinterpret_cast<bool*>(&material_component.data.is_roughness_texture_inverted));
				if (auto texture = texture_manager->GetTexture(material_component.texture_metalroughness_name); texture)
				{
					const auto* TextureID = dynamic_cast<Renderer::VulkanTextureManager*>(texture_manager.get())->vulkan_texture_imgui_descriptor_pool.GetImGuiTextureID();
					dynamic_cast<Renderer::VulkanTexture*>(texture.get())->UpdateToDescripterSet(*TextureID, 0);
					ImGui::Image(*TextureID, ImVec2(100, 100));
				}

			}
		

			is_changed |= ImGui::Checkbox("Metallic_check", reinterpret_cast<bool*>(&material_component.data.has_metalic_texture));
			if (material_component.data.has_metalic_texture)
			{
				if (ImGui::BeginCombo("MetallicTexture", material_component.texture_metalroughness_name.c_str()))
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
					const auto* TextureID = dynamic_cast<Renderer::VulkanTextureManager*>(texture_manager.get())->vulkan_texture_imgui_descriptor_pool.GetImGuiTextureID();
					dynamic_cast<Renderer::VulkanTexture*>(texture.get())->UpdateToDescripterSet(*TextureID, 0);
					ImGui::Image(*TextureID, ImVec2(100, 100));
				}
			}

			if(!material_component.data.has_metalroughness_texture  && !material_component.data.has_metalic_texture)
			{
				material_component.texture_metalroughness_name = "";
			}


			is_changed |= ImGui::Checkbox("roughness/smoothness_check", reinterpret_cast<bool*>(&material_component.data.has_roughness_texture));
			if (material_component.data.has_roughness_texture)
			{
				//ImGui::

				//const float current_item_width = treenode_size;
				static ImGuiTextFilter filter;
				static const std::string text = "Rough/Smoothness";
				float text_width = ImGui::CalcTextSize(text.c_str()).x;
				ImGui::Text(text.c_str());
				ImGui::SameLine();

				if (ImGui::BeginCombo("##"/*text.c_str()*/, material_component.texture_roughness_name.c_str()))
				{
					for (const auto& key : texture_map | std::views::keys)
					{
						if (filter.PassFilter(key.c_str()))
						{
							const bool selected = material_component.texture_roughness_name == key;

							if (ImGui::Selectable(key.c_str(), selected))
							{
								material_component.texture_roughness_name = key;
								is_changed |= true;
							}
							if (selected) {
								ImGui::SetItemDefaultFocus();
							}
						}
					}
					ImGui::EndCombo();
				}
				//ImGui::SameLine(current_item_width * 0.5f, text_width);
				//filter.Draw("Filter", current_item_width * 0.2f);
				ImGui::NewLine();

				ImGui::Checkbox("is_smoothness", reinterpret_cast<bool*>(&material_component.data.is_roughness_texture_inverted));
				if (auto texture = texture_manager->GetTexture(material_component.texture_roughness_name); texture)
				{
					const auto* TextureID = dynamic_cast<Renderer::VulkanTextureManager*>(texture_manager.get())->vulkan_texture_imgui_descriptor_pool.GetImGuiTextureID();
					dynamic_cast<Renderer::VulkanTexture*>(texture.get())->UpdateToDescripterSet(*TextureID, 0);
					ImGui::Image(*TextureID, ImVec2(100, 100));
				}
			}
			else
			{
				material_component.texture_roughness_name = "";
			}



			is_changed |= ImGui::Checkbox("AOTexture_check", reinterpret_cast<bool*>(&material_component.data.has_ao_texture));
			if(material_component.data.has_ao_texture)
			{
				if (ImGui::BeginCombo("AOTexture", material_component.texture_ao_name.c_str()))
				{
					if(ImGui::BeginPopupContextWindow())
					{
						char buf[25];
						ImGui::InputText("#Filter", buf, 25);
						ImGui::EndPopup();
					}
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
				if (auto texture = texture_manager->GetTexture(material_component.texture_ao_name))
				{
					const auto* TextureID = dynamic_cast<Renderer::VulkanTextureManager*>(texture_manager.get())->vulkan_texture_imgui_descriptor_pool.GetImGuiTextureID();
					dynamic_cast<Renderer::VulkanTexture*>(texture.get())->UpdateToDescripterSet(*TextureID, 0);
					//todo
					if(ImGui::IsItemHovered())
					{
						ImGui::Image(*TextureID, ImVec2(100, 100));
					}
				}
			}
			else
			{
				material_component.texture_ao_name = "";
			}


			is_changed |= ImGui::Checkbox("Emissive_check", reinterpret_cast<bool*>(&material_component.data.has_emissive_texture));
			if (material_component.data.has_emissive_texture)
			{
				if (ImGui::BeginCombo("EmissiveTexture", material_component.texture_emissive_name.c_str()))
				{
					for (const auto& key : texture_map | std::views::keys)
					{
						const bool selected = material_component.texture_emissive_name == key;
						if (ImGui::Selectable(key.c_str(), selected))
						{
							material_component.texture_emissive_name = key;
							is_changed |= true;
						}
						if (selected) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}

				if (auto texture = texture_manager->GetTexture(material_component.texture_emissive_name); texture)
				{
					const auto* TextureID = dynamic_cast<Renderer::VulkanTextureManager*>(texture_manager.get())->vulkan_texture_imgui_descriptor_pool.GetImGuiTextureID();
					dynamic_cast<Renderer::VulkanTexture*>(texture.get())->UpdateToDescripterSet(*TextureID, 0);
					ImGui::Image(*TextureID, ImVec2(100, 100));
				}
			}
			else
			{
				material_component.texture_emissive_name = "";
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
		LightComponent& light_component = OE::Engine::ecs_manager.GetComponent<LightComponent>(entID);
		if (ImGui::TreeNode(typeid(LightComponent).name()))
		{
			//ImGui::DragFloat3(GET_VARIABLE_NAME(light.pos), &light_component.data.position.x);
			ImGui::DragFloat3(GET_VARIABLE_NAME(light.direction), &light_component.data.dir.x, Engine::window->vulkan_imgui_manager.GetSliderSpeed());
			ImGui::ColorEdit3(GET_VARIABLE_NAME(light.diffuse), &light_component.data.diffuse.x, Engine::window->vulkan_imgui_manager.GetSliderSpeed());
			ImGui::DragFloat(GET_VARIABLE_NAME(light.inner_cut_off), &light_component.data.cutoff, Engine::window->vulkan_imgui_manager.GetSliderSpeed());
			ImGui::DragFloat(GET_VARIABLE_NAME(light.outer_cut_off), &light_component.data.out_cutoff, Engine::window->vulkan_imgui_manager.GetSliderSpeed());
			ImGui::DragFloat(GET_VARIABLE_NAME(light.falloff), &light_component.data.falloff, Engine::window->vulkan_imgui_manager.GetSliderSpeed());
			ImGui::InputInt(GET_VARIABLE_NAME(light.type), &light_component.data.type, Engine::window->vulkan_imgui_manager.GetSliderSpeed());
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
		Script* script = Engine::lua_script_manager.GetScript(ScriptType::AttatchedComponent, strID);
		if(script == nullptr)
		{
			script = Engine::lua_script_manager.CreateScript(strID, ScriptType::AttatchedComponent);
		}

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
						if(ImGui::Selectable(string.c_str(), selected, selected /*|| script_asset.first == false*/ ? ImGuiSelectableFlags_Disabled : ImGuiSelectableFlags_None))
						{
							script_component.name = string;
							script->ChangeScript(string);
							/*if(pScript)
							{
								
							}*/
						}
					}
					//if (ImGui::Selectable("##None", using_script_path.empty(), using_script_path.empty() ? ImGuiSelectableFlags_Disabled : ImGuiSelectableFlags_None))
					//{
					//	script_component.name = "";
					//	script->ChangeScript("");
					//	/*if(pScript)
					//	{

					//	}*/
					//}
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
