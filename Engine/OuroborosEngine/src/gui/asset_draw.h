#pragma once

#include <fstream>
#include <filesystem>
#include <string>
#include <common/assets.h>
#include "material_configure.h"
#include "../gui/gui_definitions.h"

//#include "engine.h"

namespace OE
{
	template<typename TAsset>
	void AssetImGuiImpl(const TAsset& asset)
	{
		ImGui::TextColored(OE::GUI::Colors::RED, "Implementation for this asset is not exist");
	}

	template<>
	inline void AssetImGuiImpl<Asset::Material>(const Asset::Material& material)
	{
		if(material.pbr_mat.GetMode() == Shared::MaterialMode::NONE_TEXTURE)
		{
			glm::vec3 albedo = material.pbr_mat.structure.non_texture->albedo;
			ImGui::ColorEdit3("albedo", &albedo.x, ImGuiColorEditFlags_NoInputs);
			float metallic = material.pbr_mat.structure.non_texture->metallic;
			float roughness = material.pbr_mat.structure.non_texture->roughness;
			float ao = material.pbr_mat.structure.non_texture->ao;
			ImGui::SliderFloat("metallic", &metallic, 0, 0, "%.3f", ImGuiSliderFlags_NoInput);
			ImGui::SliderFloat("roughness", &roughness, 0, 0, "%.3f", ImGuiSliderFlags_NoInput);
			ImGui::SliderFloat("ao", &ao, 0, 0, "%.3f", ImGuiSliderFlags_NoInput);
		}
		else
		{
			auto texture_material = material.pbr_mat.structure.texture;
			GUI::DefaultSizeBuffer buffer{ texture_material->texture_names[Shared::ALBEDO].c_str() };
			ImGui::InputText("albedo", buffer.data, buffer.size, ImGuiInputTextFlags_ReadOnly);
			buffer.Clear();
			buffer = texture_material->texture_names[Shared::NORMAL].c_str();
			ImGui::InputText("normal", buffer.data, buffer.size, ImGuiInputTextFlags_ReadOnly);
			buffer.Clear();
			if (texture_material->use_combined_metallic_roughness)
			{
				buffer.Clear();
				buffer = texture_material->texture_names[Shared::METALLIC_ROUGHNESS].c_str();
				ImGui::InputText("metallic_roughness", buffer.data, buffer.size, ImGuiInputTextFlags_ReadOnly);
			}
			else
			{
				buffer.Clear();
				buffer = texture_material->texture_names[Shared::METALLIC].c_str();
				ImGui::InputText("metallic", buffer.data, buffer.size, ImGuiInputTextFlags_ReadOnly);

				buffer.Clear();
				buffer = texture_material->texture_names[Shared::ROUGHNESS].c_str();
				ImGui::InputText("roughness", buffer.data, buffer.size, ImGuiInputTextFlags_ReadOnly);

				bool invert = texture_material->is_smoothness;
				ImGui::Checkbox("invert", &invert);
			}

			buffer.Clear();
			buffer = texture_material->texture_names[Shared::AO].c_str();
			ImGui::InputText("ao", buffer.data, buffer.size, ImGuiInputTextFlags_ReadOnly);

			buffer.Clear();
			buffer = texture_material->texture_names[Shared::EMISSIVE].c_str();
			ImGui::InputText("emissive", buffer.data, buffer.size, ImGuiInputTextFlags_ReadOnly);

			if (ImGui::SmallButton("Change"))
			{
				Engine::gui_manager.AddPanel(new GUI::MaterialConfigure(std::string("Change material ") + material.name, material.name));
			}
		}
	}

	template<>
	inline void AssetImGuiImpl<Asset::Image>(const Asset::Image& image)
	{
		ImGui::TextWrapped("Width : %d", image.width); ImGui::SameLine();
		ImGui::TextWrapped("Height : %d", image.height);

		const auto& texture_manager = Engine::Get().window->GetWindowData().RenderContextData->texture_manager;

		if(auto texture = texture_manager->GetTexture(image.filename); texture)
		{
			const auto* TextureID = dynamic_cast<Renderer::VulkanTextureManager*>(texture_manager.get())->vulkan_texture_imgui_descriptor_pool.GetImGuiTextureID();
			dynamic_cast<Renderer::VulkanTexture*>(texture.get())->UpdateToDescripterSet(*TextureID, 0);
			ImGui::Image(*TextureID, ImVec2(100, 100));
		}
	}

	template<>
	inline void AssetImGuiImpl<Asset::Mesh>(const Asset::Mesh& mesh)
	{
		for (const auto& data : mesh.payload_datas)
		{
			const std::string name = data.first;
			ImGui::Text(name.c_str());
		}
		//ImGui::TextWrapped("Width : %d", image.width); ImGui::SameLine();
		//ImGui::TextWrapped("Height : %d", image.height);

		//const auto& texture_manager = Engine::Get().window->GetWindowData().RenderContextData->texture_manager;

		//if (auto texture = texture_manager->GetTexture(image.filename); texture)
		//{
		//	const auto* TextureID = dynamic_cast<Renderer::VulkanTextureManager*>(texture_manager.get())->vulkan_texture_imgui_descriptor_pool.GetImGuiTextureID();
		//	dynamic_cast<Renderer::VulkanTexture*>(texture.get())->UpdateToDescripterSet(*TextureID, 0);
		//	ImGui::Image(*TextureID, ImVec2(100, 100));
		//}
	}

	template<>
	inline void AssetImGuiImpl<Asset::Script>(const Asset::Script& script)
	{
		const std::filesystem::path script_path(script.path);
		const std::string& extension = script_path.extension().string();

		std::ifstream script_file;

		script_file.open(script_path);

		if (script_file.is_open())
		{
			std::string line;
			while (std::getline(script_file, line))
			{
				//if (line.contains("--"))
				//{
				//	ImGui::TextColored(OE::GUI::Colors::GREEN, line.c_str());
				//}

				//else
				//{
					ImGui::Text(line.c_str());
				//}

			}
		}
		else
		{
			ImGui::TextColored(OE::GUI::Colors::RED, "Cannot open file %s", script_path.c_str());
		}

		if (ImGui::SmallButton("Reload changes"))
		{
			auto type = ScriptAssetManager::GetTypeFromExtension(extension);
			const auto pScript = OE::Engine::lua_script_manager.GetScript(type, script_path.string());
			pScript->ChangeScript(script_path.string());

			if(type == Script::ScriptType::Component)
			{
				//OE::Engine::ecs_manager.ForEntitiesMatching<ScriptingSystem>(0.f, [script_path](OE::Status status, OE::ecs_ID ent, float dt, ScriptComponent& script_component)
				//{
				//	if(script_component.name == script_path)
				//	{
				//		const auto pScript = OE::Engine::lua_script_manager.GetScript(Script::ScriptType::AttatchedComponent, std::to_string(ent));
				//		if(pScript)
				//		{
				//			pScript->ChangeScript(script_path.string());
				//		}
				//	}
				//}
				//);
			}


		}
		//ImGui::TextWrapped(.c_str());
	}
}
