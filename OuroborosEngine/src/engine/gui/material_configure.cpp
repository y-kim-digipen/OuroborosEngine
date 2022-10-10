#include "material_configure.h"

#include "wrapped_functions.h"
#include "../src/engine/engine.h"

OE::GUI::MaterialConfigure::MaterialConfigure(const std::string& title, const std::string& material_name) :
	GUI_Base(title), reference_material(material_name)
{
	const auto& material_manager = Engine::window->GetWindowData().RenderContextData->material_manager;
	const auto& texture_manager = Engine::window->GetWindowData().RenderContextData->texture_manager;

	using_texture = false;

	is_exist_material = material_name.empty() == false;
	if(is_exist_material)
	{
		Renderer::Material* material = material_manager->GetMaterial(reference_material);
		using_texture =
			material->GetMaterialData()->has_albedo_texture ||
			material->GetMaterialData()->has_normal_texture ||
			material->GetMaterialData()->has_metalroughness_texture ||
			material->GetMaterialData()->has_metalic_texture ||
			material->GetMaterialData()->has_roughness_texture ||
			material->GetMaterialData()->has_ao_texture ||
			material->GetMaterialData()->has_emissive_texture;
	}

	if(is_exist_material)
	{
		Renderer::Material* material = material_manager->GetMaterial(reference_material);
		std::ranges::copy(material_name, material_name_buf.data);
		material_name_buf.data[material_name.length()] = '\0';
		if (using_texture)
		{
			pbr_material_info.SwitchMode(Shared::MaterialMode::TEXTURE);
			std::string albedo_texture_name = texture_manager->GetName(material->GetTexture(Shared::ALBEDO));
			pbr_material_info.structure.texture->texture_names[Shared::ALBEDO] = texture_manager->GetName(material->GetTexture(Shared::ALBEDO));
			pbr_material_info.structure.texture->texture_names[Shared::NORMAL] = texture_manager->GetName(material->GetTexture(Shared::NORMAL));
			pbr_material_info.structure.texture->texture_names[Shared::METALLIC_ROUGHNESS] = texture_manager->GetName(material->GetTexture(Shared::METALLIC_ROUGHNESS));
			pbr_material_info.structure.texture->texture_names[Shared::METALLIC] = texture_manager->GetName(material->GetTexture(Shared::METALLIC));
			pbr_material_info.structure.texture->texture_names[Shared::ROUGHNESS] = texture_manager->GetName(material->GetTexture(Shared::ROUGHNESS));
			pbr_material_info.structure.texture->texture_names[Shared::AO] = texture_manager->GetName(material->GetTexture(Shared::AO));
			pbr_material_info.structure.texture->texture_names[Shared::EMISSIVE] = texture_manager->GetName(material->GetTexture(Shared::EMISSIVE));

			texture_names = pbr_material_info.structure.texture->texture_names;

			pbr_material_info.structure.texture->is_smoothness = material->GetMaterialData()->is_smoothness;
			pbr_material_info.structure.texture->use_combined_metallic_roughness =
				!pbr_material_info.structure.texture->texture_names[Shared::METALLIC_ROUGHNESS].empty() &&
				pbr_material_info.structure.texture->texture_names[Shared::METALLIC].empty() &&
				pbr_material_info.structure.texture->texture_names[Shared::ROUGHNESS].empty();

			invert_roughness = pbr_material_info.structure.texture->is_smoothness;
			use_metallic_roughness = pbr_material_info.structure.texture->use_combined_metallic_roughness;
		}
		else
		{
			pbr_material_info.structure.non_texture->albedo = material->GetMaterialData()->albedo;
			pbr_material_info.structure.non_texture->ao = material->GetMaterialData()->ao;
			pbr_material_info.structure.non_texture->metallic = material->GetMaterialData()->metallic;
			pbr_material_info.structure.non_texture->roughness = material->GetMaterialData()->roughness;
		}
	}
}

void OE::GUI::MaterialConfigure::Draw()
{
	draw_internal();
}

void OE::GUI::MaterialConfigure::draw_internal()
{
	const auto& render_context_data = Engine::window->GetWindowData().RenderContextData;

	auto& texture_manager = render_context_data->texture_manager;
	auto& material_manager = render_context_data->material_manager;
	auto& material_datum = material_manager->GetMaterialDatum();

	ImGui::Begin(name.c_str(), &open);
	{
		const bool has_same_name = material_datum.contains(material_name_buf.data);
		ImGui::InputTextWithHint("##name", is_exist_material ? reference_material.c_str() : "name", material_name_buf.data, material_name_buf.size);
		ImGui::SameLine();
		if (!is_exist_material && has_same_name)
		{
			ImGui::TextColored(ImVec4(0.6f, 0.f, 0.f, 1.f), (std::string("Already exist") + material_name_buf.data).c_str());
		}
		else if (std::string{ material_name_buf.data }.empty())
		{
			/*ImGui::TextColored(ImVec4(0.6f, 0.f, 0.f, 1.f), (std::string("Already exist") + material_name_buf.data).c_str());*/
		}
		else
		{
			if (ImGui::Button("Save"))
			{
				//Asset::MaterialData material_data;
				Asset::Material material;
				Shared::MaterialMode mode = using_texture ? Shared::MaterialMode::TEXTURE : Shared::MaterialMode::NONE_TEXTURE;
				material.pbr_mat.SwitchMode(mode);
				if (using_texture == false)
				{
					material.pbr_mat.structure.non_texture->albedo = albedo;
					material.pbr_mat.structure.non_texture->metallic = metallic;
					material.pbr_mat.structure.non_texture->roughness = roughness;
					material.pbr_mat.structure.non_texture->ao = ao;
				}
				else
				{
					material.pbr_mat.structure.texture->texture_names = texture_names;
					material.pbr_mat.structure.texture->is_smoothness = invert_roughness;
					//material_data.has_albedo_texture = !texture_names[Shared::ALBEDO].empty();
					//material_data.has_normal_texture = !texture_names[Shared::NORMAL].empty();
					//material_data.has_metalroughness_texture = !texture_names[Shared::METALLIC_ROUGHNESS].empty();
					//material_data.has_metalic_texture = !texture_names[Shared::METALLIC].empty();
					//material_data.has_roughness_texture = !texture_names[Shared::ROUGHNESS].empty();
					//material_data.has_ao_texture = !texture_names[Shared::AO].empty();
					//material_data.has_emissive_texture = !texture_names[Shared::EMISSIVE].empty();
					//material_data.is_smoothness = invert_roughness;
				}

				//material_component.data = material_data;
				if (is_exist_material == false)
				{
					Engine::asset_manager.GetManager<MaterialAssetManager>().LoadAsset(material_name_buf.data, material);
					//material_manager->AddMaterial(material_name_buf.data, material_data);
				}
				else
				{
					//material_manager->ChangeMaterialName(reference_material, material_name_buf.data);
					//reference_material = material_name_buf.data;
				}

				//if (using_texture)
				//{
				//	auto& texture_manager = Engine::Get().window->GetWindowData().RenderContextData->texture_manager;
				//	auto material = material_manager->GetMaterial(is_exist_material ? reference_material : material_name_buf.data);

				//	for(int type = 0; type < Shared::COUNT; ++type)
				//	{
				//		if (material->DoUseTexture(static_cast<Shared::PBR_TEXTURE_TYPES>(type)))
				//		{
				//			//material_component.texture_albedo_name = albedo_texture_name;
				//			material->SetTexture(static_cast<Shared::PBR_TEXTURE_TYPES>(type), texture_manager->GetTexture(texture_names[type]));
				//		}
				//	}

				//	
				//}
				open = false;
			}
			filter.Draw();
			ImGui::Checkbox("Using Texture", &using_texture);
			ImGui::Separator();
			if (using_texture)
			{
				if (ImGui::Checkbox("combine metallic roughness", &use_metallic_roughness))
				{
					if (use_metallic_roughness)
					{
						texture_names[Shared::METALLIC].clear();
						texture_names[Shared::NORMAL].clear();
					}
					else
					{
						texture_names[Shared::METALLIC_ROUGHNESS].clear();
					}
				}
				ImGui::Separator();
				OE::GUI_Input::TextureSelectable("albedo", texture_names[Shared::ALBEDO], &filter);
				ImGui::Separator();
				OE::GUI_Input::TextureSelectable("normal", texture_names[Shared::NORMAL], &filter);
				ImGui::Separator();

				if (use_metallic_roughness)
				{
					OE::GUI_Input::TextureSelectable("metallic / roughness", texture_names[Shared::METALLIC_ROUGHNESS], &filter);
					ImGui::Separator();
				}
				else
				{
					OE::GUI_Input::TextureSelectable("metallic", texture_names[Shared::METALLIC], &filter);
					ImGui::Separator();
					OE::GUI_Input::TextureSelectable("roughness", texture_names[Shared::ROUGHNESS], &filter);
					ImGui::Checkbox("Invert", &invert_roughness);
					ImGui::Separator();
				}

				OE::GUI_Input::TextureSelectable("ao", texture_names[Shared::AO], &filter);
				ImGui::Separator();
				OE::GUI_Input::TextureSelectable("emissive", texture_names[Shared::EMISSIVE], &filter);
			}
			else
			{
				ImGui::ColorEdit3("albedo", &albedo.x);
				GUI_Input::DragFloat("metallic", &metallic, 0.0, 1.0);
				GUI_Input::DragFloat("roughness", &roughness, 0.0f, 1.0f);
				GUI_Input::DragFloat("ao", &ao, 0.0, 1.0);
			}
		}
		ImGui::End();

		if (open == false)
		{
			Engine::gui_manager.RemovePanel(this);
		}
	}
}

