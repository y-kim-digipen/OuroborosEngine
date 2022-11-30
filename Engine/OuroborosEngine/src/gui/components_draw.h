#pragma once
#include "wrapped_functions.h"
#include "material_configure.h"
#include <filesystem>
#include "gizmo_editor.h"
namespace OE
{
	template<typename TComponent>
	void ComponentDrawFunction(ecs_ID entID)
	{
		ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Component %s detail is not implemented", typeid(TComponent).name());
	}

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
			glm::vec3 position = transform_component.GetPosition();
			if(GUI_Input::DragFloat3("Pos", &position.x))
			{
				transform_component.SetPosition(position);
			}

			glm::vec3 rotation = transform_component.GetRotation();
			if(GUI_Input::DragFloat3("Rot", &rotation.x))
			{
				transform_component.SetRotation(rotation);
			}
			glm::vec3 scale = transform_component.GetScale();
			if(GUI_Input::DragFloat3("Scale", &scale.x))
			{
				transform_component.SetScale(scale);
			}
			if(ImGui::Button("Gizmo"))
			{
				auto gizmo_editing_panel = Engine::gui_manager.TryGetPanel<OE::GUI::GizmoEditor>();
				if(gizmo_editing_panel == nullptr)
				{
					bool always_true = true;
					Engine::gui_manager.RunBehavior({ "ECS", "Edit" }, "Gizmo Editor");
				}
				OE::GUI::GizmoEditor::SetSelectedEntity(entID);
			}

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
			strcpy_s(buf, tag_component.tag.c_str());

			if (ImGui::InputText("##Tag", buf, BufSize, ImGuiInputTextFlags_EnterReturnsTrue))
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

			/*	if (ImGui::Button("Reload"))
				{
					Engine::RegisterEvent(Engine::PRE, std::bind(&Renderer::VulkanShader::Reload, shader));
				}*/

				for (auto& member_variable : shader->binding_block_members) {
					bool is_changed = false;
						switch (member_variable.second.type) {
						case Renderer::DataType::BOOL:
							is_changed |= ImGui::Checkbox(member_variable.first.c_str(), (bool*)shader->GetMemberVariable(member_variable.first));
							break;
						case Renderer::DataType::FLOAT:
							is_changed |= ImGui::DragFloat(member_variable.first.c_str(), (float*)shader->GetMemberVariable(member_variable.first), Engine::gui_manager.GetSliderSpeed());
							break;
						case Renderer::DataType::FLOAT2:
							is_changed |= ImGui::DragFloat2(member_variable.first.c_str(), (float*)shader->GetMemberVariable(member_variable.first), Engine::gui_manager.GetSliderSpeed());
							break;
						case Renderer::DataType::FLOAT3:
							is_changed |= ImGui::DragFloat3(member_variable.first.c_str(), (float*)shader->GetMemberVariable(member_variable.first), Engine::gui_manager.GetSliderSpeed());
							break;
						case Renderer::DataType::FLOAT4:
							is_changed |= ImGui::DragFloat4(member_variable.first.c_str(), (float*)shader->GetMemberVariable(member_variable.first), Engine::gui_manager.GetSliderSpeed());
							break;
						case Renderer::DataType::INT:
							is_changed |= ImGui::DragInt(member_variable.first.c_str(), (int*)shader->GetMemberVariable(member_variable.first));
							break;
						case Renderer::DataType::INT2:
							is_changed |= ImGui::DragInt2(member_variable.first.c_str(), (int*)shader->GetMemberVariable(member_variable.first));
							break;
						case Renderer::DataType::INT3:
							is_changed |= ImGui::DragInt3(member_variable.first.c_str(), (int*)shader->GetMemberVariable(member_variable.first));
							break;
						case Renderer::DataType::INT4:	
							is_changed |= ImGui::DragInt4(member_variable.first.c_str(), (int*)shader->GetMemberVariable(member_variable.first));
							break;
						case Renderer::DataType::MAT3:
							break;
						case Renderer::DataType::MAT4:
							break;
						case Renderer::DataType::SAMPLER2D:
							if (ImGui::BeginCombo("Textures", member_variable.second.texture_name))
							{
								const auto& texture_map = Engine::Get().asset_manager.GetManager<ImageAssetManager>().GetAssetRawData();
								const auto& texture_manager = Engine::window->GetWindowData().RenderContextData->texture_manager;
								for (const auto& key : texture_map | std::views::keys)
								{
									const bool selected = member_variable.second.texture_name == key;
									if (ImGui::Selectable(key.c_str(), selected))
									{
										member_variable.second.texture_name = key.c_str();
										shader->SetUniformTexture(member_variable.first.c_str(), texture_manager->GetTexture(key));
									}
									if (selected) {
										ImGui::SetItemDefaultFocus();
									}
								}
								ImGui::EndCombo();
							}
						}

						if (is_changed) {
							shader->SetUniformValue(member_variable.first.c_str(), shader->GetMemberVariable(member_variable.first));
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

			const auto& material_datum = Engine::window->GetWindowData().RenderContextData->material_manager->GetMaterialDatum();
			if(ImGui::BeginCombo("Material", material_component.name.c_str()))
			{
				for (const auto& material : material_datum)
				{
					const char* material_name = material.first.c_str();
					if(ImGui::Selectable(material_name, material_name == material_component.name))
					{
						material_component.name = material_name;
						//Engine::gui_manager.AddPanel(new GUI::MaterialConfigure(std::string("Change material ") + material_name, material_name));
					}
				}

				if (ImGui::Selectable("New"))
				{
					Engine::gui_manager.RunBehavior({ "File", "Asset" }, "New material");
				}
				ImGui::EndCombo();
			}
			//	ImGui::Checkbox("Using Texture", &using_texture);
			//	ImGui::Separator();
			//	if(using_texture)
			//	{
			//		if(ImGui::Checkbox("combine metallic roughness", &use_metallic_roughness))
			//		{
			//			if (use_metallic_roughness)
			//			{
			//				metallic_texture_name.clear();
			//				roughness_texture_name.clear();
			//			}
			//			else
			//			{
			//				metallic_roughness_texture_name.clear();
			//			}
			//		}
			//		ImGui::Separator();
			//		OE::GUI_Input::TextureSelectable("albedo", albedo_texture_name);
			//		ImGui::Separator();
			//		OE::GUI_Input::TextureSelectable("normal", normal_texture_name);
			//		ImGui::Separator();

			//		if(use_metallic_roughness)
			//		{
			//			OE::GUI_Input::TextureSelectable("metallic / roughness", metallic_roughness_texture_name);
			//			ImGui::Separator();
			//		}
			//		else
			//		{
			//			OE::GUI_Input::TextureSelectable("metallic", metallic_texture_name);
			//			ImGui::Separator();
			//			OE::GUI_Input::TextureSelectable("roughness", roughness_texture_name);
			//			ImGui::Checkbox("Invert", &invert_roughness);
			//			ImGui::Separator();
			//		}

			//		OE::GUI_Input::TextureSelectable("ao", ao_texture_name);
			//		ImGui::Separator();
			//		OE::GUI_Input::TextureSelectable("emissive", emissive_texture_name);
			//	}
			//	else
			//	{
			//		ImGui::ColorEdit3("albedo", &albedo.x);
			//		GUI_Input::DragFloat("metallic", &metallic, 0.0, 1.0);
			//		GUI_Input::DragFloat("roughness", &roughness, 0.0f, 1.0f);
			//		GUI_Input::DragFloat("ao", &ao, 0.0, 1.0);
			//	}

			//	ImGui::End();
			//}
			//else
			//{
			//	memset(buffer, 0, 30);
			//}


			//bool is_changed = ImGui::ColorEdit3(GET_VARIABLE_NAME(material.albedo), &material_component.data.albedo.x);

			//is_changed |=  GUI_Input::DragFloat3("metallic", &material_component.data.metallic, 0.0, 1.0);
			//is_changed |=  GUI_Input::DragFloat3("roughness", &material_component.data.roughness, 0.0f, 1.0f);
			//is_changed |=  GUI_Input::DragFloat3("ao", &material_component.data.ao, 0.0, 1.0);

			//if (ImGui::Button("save"))
			//{
			//	material_component.is_save = true;
			//}

			//const auto& texture_manager = Engine::Get().window->GetWindowData().RenderContextData->texture_manager;

			//is_changed |= ImGui::Checkbox("AlbedoTexture_check", reinterpret_cast<bool*>(&material_component.data.has_albedo_texture));

			//if (material_component.data.has_albedo_texture)
			//{
			//	if (ImGui::BeginCombo("AlbedoTexture", material_component.texture_albedo_name.c_str()))
			//	{
			//		for (const auto& key : texture_map | std::views::keys)
			//		{
			//			const bool selected = material_component.texture_albedo_name == key;
			//			if (ImGui::Selectable(key.c_str(), selected))
			//			{
			//				material_component.texture_albedo_name = key;
			//				is_changed |= true;

			//			}
			//			if (selected) {
			//				ImGui::SetItemDefaultFocus();
			//			}
			//		}
			//		ImGui::EndCombo();
			//	}

			//	if (auto texture = texture_manager->GetTexture(material_component.texture_albedo_name); texture)
			//	{
			//		const auto* TextureID = dynamic_cast<Renderer::VulkanTextureManager*>(texture_manager.get())->vulkan_texture_imgui_descriptor_pool.GetImGuiTextureID();
			//		dynamic_cast<Renderer::VulkanTexture*>(texture.get())->UpdateToDescripterSet(*TextureID, 0);
			//		ImGui::Image(*TextureID, ImVec2(100, 100));
			//	}
			//}
			//else
			//{
			//	material_component.texture_albedo_name = "";
			//}
			////ImGui::EndMEnu
			//
			//

			//is_changed |= ImGui::Checkbox("NormalTexture_check", reinterpret_cast<bool*>(&material_component.data.has_normal_texture));
			//if (material_component.data.has_normal_texture)
			//{
			//	if (ImGui::BeginCombo("NormalTexture", material_component.texture_normal_name.c_str()))
			//	{
			//		for (const auto& key : texture_map | std::views::keys)
			//		{
			//			const bool selected = material_component.texture_normal_name == key;
			//			if (ImGui::Selectable(key.c_str(), selected))
			//			{
			//				material_component.texture_normal_name = key;
			//				is_changed |= true;
			//			}
			//			if (selected) {
			//				ImGui::SetItemDefaultFocus();
			//			}
			//		}
			//		ImGui::EndCombo();
			//	}
			//	if (auto texture = texture_manager->GetTexture(material_component.texture_normal_name); texture)
			//	{
			//		const auto* TextureID = dynamic_cast<Renderer::VulkanTextureManager*>(texture_manager.get())->vulkan_texture_imgui_descriptor_pool.GetImGuiTextureID();
			//		dynamic_cast<Renderer::VulkanTexture*>(texture.get())->UpdateToDescripterSet(*TextureID, 0);
			//		ImGui::Image(*TextureID, ImVec2(100, 100));
			//	}
			//}
			//else
			//{
			//	material_component.texture_normal_name = "";
			//}
			//


			//is_changed |= ImGui::Checkbox("MetalRoughnessTexture_check", reinterpret_cast<bool*>(&material_component.data.has_metalroughness_texture));
			//if (material_component.data.has_metalroughness_texture)
			//{
			//	if (ImGui::BeginCombo("MetalRoughnessTexture", material_component.texture_metalroughness_name.c_str()))
			//	{
			//		for (const auto& key : texture_map | std::views::keys)
			//		{
			//			const bool selected = material_component.texture_metalroughness_name == key;
			//			if (ImGui::Selectable(key.c_str(), selected))
			//			{
			//				material_component.texture_metalroughness_name = key;
			//				is_changed |= true;
			//			}
			//			if (selected) {
			//				ImGui::SetItemDefaultFocus();
			//			}
			//		}
			//		ImGui::EndCombo();
			//	}
			//	ImGui::Checkbox("is_smoothness ", reinterpret_cast<bool*>(&material_component.data.is_smoothness));
			//	if (auto texture = texture_manager->GetTexture(material_component.texture_metalroughness_name); texture)
			//	{
			//		const auto* TextureID = dynamic_cast<Renderer::VulkanTextureManager*>(texture_manager.get())->vulkan_texture_imgui_descriptor_pool.GetImGuiTextureID();
			//		dynamic_cast<Renderer::VulkanTexture*>(texture.get())->UpdateToDescripterSet(*TextureID, 0);
			//		ImGui::Image(*TextureID, ImVec2(100, 100));
			//	}

			//}
		

			//is_changed |= ImGui::Checkbox("Metallic_check", reinterpret_cast<bool*>(&material_component.data.has_metalic_texture));
			//if (material_component.data.has_metalic_texture)
			//{
			//	if (ImGui::BeginCombo("MetallicTexture", material_component.texture_metalroughness_name.c_str()))
			//	{
			//		for (const auto& key : texture_map | std::views::keys)
			//		{
			//			const bool selected = material_component.texture_metalroughness_name == key;
			//			if (ImGui::Selectable(key.c_str(), selected))
			//			{
			//				material_component.texture_metalroughness_name = key;
			//				is_changed |= true;
			//			}
			//			if (selected) {
			//				ImGui::SetItemDefaultFocus();
			//			}
			//		}
			//		ImGui::EndCombo();
			//	}

			//	if (auto texture = texture_manager->GetTexture(material_component.texture_metalroughness_name); texture)
			//	{
			//		const auto* TextureID = dynamic_cast<Renderer::VulkanTextureManager*>(texture_manager.get())->vulkan_texture_imgui_descriptor_pool.GetImGuiTextureID();
			//		dynamic_cast<Renderer::VulkanTexture*>(texture.get())->UpdateToDescripterSet(*TextureID, 0);
			//		ImGui::Image(*TextureID, ImVec2(100, 100));
			//	}
			//}

			//if(!material_component.data.has_metalroughness_texture  && !material_component.data.has_metalic_texture)
			//{
			//	material_component.texture_metalroughness_name = "";
			//}


			//is_changed |= ImGui::Checkbox("roughness/smoothness_check", reinterpret_cast<bool*>(&material_component.data.has_roughness_texture));
			//if (material_component.data.has_roughness_texture)
			//{
			//	//ImGui::

			//	//const float current_item_width = treenode_size;
			//	static ImGuiTextFilter filter;
			//	static const std::string text = "Rough/Smoothness";
			//	float text_width = ImGui::CalcTextSize(text.c_str()).x;
			//	ImGui::Text(text.c_str());
			//	ImGui::SameLine();

			//	if (ImGui::BeginCombo("##"/*text.c_str()*/, material_component.texture_roughness_name.c_str()))
			//	{
			//		for (const auto& key : texture_map | std::views::keys)
			//		{
			//			if (filter.PassFilter(key.c_str()))
			//			{
			//				const bool selected = material_component.texture_roughness_name == key;

			//				if (ImGui::Selectable(key.c_str(), selected))
			//				{
			//					material_component.texture_roughness_name = key;
			//					is_changed |= true;
			//				}
			//				if (selected) {
			//					ImGui::SetItemDefaultFocus();
			//				}
			//			}
			//		}
			//		ImGui::EndCombo();
			//	}
			//	//ImGui::SameLine(current_item_width * 0.5f, text_width);
			//	//filter.Draw("Filter", current_item_width * 0.2f);
			//	ImGui::NewLine();

			//	ImGui::Checkbox("is_smoothness", reinterpret_cast<bool*>(&material_component.data.is_smoothness));
			//	if (auto texture = texture_manager->GetTexture(material_component.texture_roughness_name); texture)
			//	{
			//		const auto* TextureID = dynamic_cast<Renderer::VulkanTextureManager*>(texture_manager.get())->vulkan_texture_imgui_descriptor_pool.GetImGuiTextureID();
			//		dynamic_cast<Renderer::VulkanTexture*>(texture.get())->UpdateToDescripterSet(*TextureID, 0);
			//		ImGui::Image(*TextureID, ImVec2(100, 100));
			//	}
			//}
			//else
			//{
			//	material_component.texture_roughness_name = "";
			//}



			//is_changed |= ImGui::Checkbox("AOTexture_check", reinterpret_cast<bool*>(&material_component.data.has_ao_texture));
			//if(material_component.data.has_ao_texture)
			//{
			//	if (ImGui::BeginCombo("AOTexture", material_component.texture_ao_name.c_str()))
			//	{
			//		if(ImGui::BeginPopupContextWindow())
			//		{
			//			char buf[25];
			//			ImGui::InputText("#Filter", buf, 25);
			//			ImGui::EndPopup();
			//		}
			//		for (const auto& key : texture_map | std::views::keys)
			//		{
			//			const bool selected = material_component.texture_ao_name == key;
			//			if (ImGui::Selectable(key.c_str(), selected))
			//			{
			//			material_component.texture_ao_name = key;
			//			is_changed |= true;
			//			}
			//			if (selected) {
			//			ImGui::SetItemDefaultFocus();
			//			}
			//		}
			//		ImGui::EndCombo();
			//	}
			//	if (auto texture = texture_manager->GetTexture(material_component.texture_ao_name))
			//	{
			//		const auto* TextureID = dynamic_cast<Renderer::VulkanTextureManager*>(texture_manager.get())->vulkan_texture_imgui_descriptor_pool.GetImGuiTextureID();
			//		dynamic_cast<Renderer::VulkanTexture*>(texture.get())->UpdateToDescripterSet(*TextureID, 0);
			//		//todo
			//		if(ImGui::IsItemHovered())
			//		{
			//			ImGui::Image(*TextureID, ImVec2(100, 100));
			//		}
			//	}
			//}
			//else
			//{
			//	material_component.texture_ao_name = "";
			//}


			//is_changed |= ImGui::Checkbox("Emissive_check", reinterpret_cast<bool*>(&material_component.data.has_emissive_texture));
			//if (material_component.data.has_emissive_texture)
			//{
			//	if (ImGui::BeginCombo("EmissiveTexture", material_component.texture_emissive_name.c_str()))
			//	{
			//		for (const auto& key : texture_map | std::views::keys)
			//		{
			//			const bool selected = material_component.texture_emissive_name == key;
			//			if (ImGui::Selectable(key.c_str(), selected))
			//			{
			//				material_component.texture_emissive_name = key;
			//				is_changed |= true;
			//			}
			//			if (selected) {
			//				ImGui::SetItemDefaultFocus();
			//			}
			//		}
			//		ImGui::EndCombo();
			//	}

			//	if (auto texture = texture_manager->GetTexture(material_component.texture_emissive_name); texture)
			//	{
			//		const auto* TextureID = dynamic_cast<Renderer::VulkanTextureManager*>(texture_manager.get())->vulkan_texture_imgui_descriptor_pool.GetImGuiTextureID();
			//		dynamic_cast<Renderer::VulkanTexture*>(texture.get())->UpdateToDescripterSet(*TextureID, 0);
			//		ImGui::Image(*TextureID, ImVec2(100, 100));
			//	}
			//}
			//else
			//{
			//	material_component.texture_emissive_name = "";
			//}


			//

			//if (is_changed)
			//{
			//	material_component.flag = true;
			//}
			
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
			//ImGui::DragFloat3(GET_VARIABLE_NAME(light.position), &light_component.data.position.x);
			ImGui::DragFloat3(GET_VARIABLE_NAME(light.direction), &light_component.data.dir.x, Engine::gui_manager.GetSliderSpeed());
			ImGui::ColorEdit3(GET_VARIABLE_NAME(light.diffuse), &light_component.data.diffuse.x, Engine::gui_manager.GetSliderSpeed());
			ImGui::DragFloat(GET_VARIABLE_NAME(light.inner_cut_off), &light_component.data.cutoff, Engine::gui_manager.GetSliderSpeed());
			ImGui::DragFloat(GET_VARIABLE_NAME(light.outer_cut_off), &light_component.data.out_cutoff, Engine::gui_manager.GetSliderSpeed());
			ImGui::DragFloat(GET_VARIABLE_NAME(light.falloff), &light_component.data.falloff, Engine::gui_manager.GetSliderSpeed());
			ImGui::InputInt(GET_VARIABLE_NAME(light.type), &light_component.data.type, Engine::gui_manager.GetSliderSpeed());
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
			//ImGui::DragFloat3(GET_VARIABLE_NAME(light.position), &light_component.data.position.x);
			//ImGui::DragFloat3(GET_VARIABLE_NAME(light.direction), &light_component.data.direction.x);
			ImGui::TreePop();
		}

	}
}