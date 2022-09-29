#pragma once
#include <string>
#include <imgui-docking/imgui.h>

#include "../src/engine/engine.h"

namespace OE
{
	namespace GUI_Input
	{
		template<typename T>
		static void ImGuiDragDropCopy(T& source_and_target, ImGuiDragDropFlags flag = ImGuiDragDropFlags_None)
		{
			if (ImGui::BeginDragDropSource(flag))
			{
				ImGui::SetDragDropPayload(typeid(T).name(), &source_and_target, sizeof(T));
				ImGui::Text("Copy");
				ImGui::EndDragDropSource();
			}
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(typeid(T).name()))
				{
					const T payload_data = *static_cast<const T*>(payload->Data);
					source_and_target = payload_data;
				}
				ImGui::EndDragDropTarget();
			}
		};

		inline bool DragFloat(const std::string& name, float* val, float min = 0, float max = 0)
		{
			return ImGui::DragFloat(name.c_str(), val, OE::Engine::window->vulkan_imgui_manager.slider_speed, min, max);
		}

		inline bool DragFloat3(const std::string& name, float* val, float min = 0, float max = 0)
		{
			return ImGui::DragFloat3(name.c_str(), val, OE::Engine::window->vulkan_imgui_manager.slider_speed, min, max);
		}

		inline void Image(ImTextureID tex_id, bool hover = false, ImVec2 size = ImVec2())
		{
			if (hover)
			{
				if (ImGui::IsItemHovered())
				{
					ImGui::Image(tex_id, size);
				}
			}
			else
			{
				ImGui::Image(tex_id, size);
			}
		}

		inline void TextureSelectable(const std::string& label, std::string& texture_name)
		{
			const auto& texture_map = Engine::Get().asset_manager.GetManager<ImageAssetManager>().GetAssetRawData();

			static ImGuiTextFilter filter;
			filter.Draw();
			if (ImGui::BeginCombo(label.c_str(), texture_name.c_str()))
			{
				if (ImGui::Selectable("##", texture_name.empty()))
				{
					texture_name.clear();
				}
				for (const auto& key : texture_map | std::views::keys)
				{
					if (filter.PassFilter(key.c_str()))
					{
						const bool selected = texture_name == key;

						if (ImGui::Selectable(key.c_str(), selected))
						{
							texture_name = key;
						}
						if (selected) {
							ImGui::SetItemDefaultFocus();
						}
					}
				}
				ImGui::EndCombo();
			}
			if(!texture_name.empty())
			{
				const auto& texture_manager = Engine::Get().window->GetWindowData().RenderContextData->texture_manager_;
				if (const auto texture = texture_manager->GetTexture(texture_name))
				{
					const auto* TextureID = dynamic_cast<Renderer::VulkanTextureManager*>(texture_manager.get())->vulkan_texture_imgui_descriptor_pool.GetImGuiTextureID();
					dynamic_cast<Renderer::VulkanTexture*>(texture.get())->UpdateToDescripterSet(*TextureID, 0);
					//todo
					if (ImGui::IsItemHovered())
					{
						ImGui::Image(*TextureID, ImVec2(100, 100));
					}
				}
			}
		}

		//inline void InputText()
		//{
		//	ImGui::ColorEdit4()
		//}
	}

}
