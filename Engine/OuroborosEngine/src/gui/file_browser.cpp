#include "file_browser.h"

#include <filesystem>
#include <vector>

#include "../core/engine.h"

namespace OE
{
	namespace GUI
	{
		void FileBrowser::Draw()
		{
			ImGui::Begin(name.c_str(), &open);
			static std::string directory_path = "..\\";

			static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;
			static std::vector<bool> row_selected(200);
			if (ImGui::BeginTable("", 2, flags))
			{
				ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableHeadersRow();
				int table_idx = 0;
				for (const auto& entry : std::filesystem::directory_iterator(directory_path))
				{
					ImGui::TableNextRow();

					const auto& path = entry.path();
					auto relative_path = std::filesystem::relative(path, directory_path);

					ImGui::TableSetColumnIndex(0);

					if (ImGui::Selectable(entry.is_directory() ? (relative_path.string() + '\\').c_str() : relative_path.string().c_str(), row_selected[table_idx]), ImGuiSelectableFlags_AllowDoubleClick)
					{
						if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && ImGui::IsItemHovered())
						{
							if (entry.is_directory())
							{
								directory_path = entry.path().string();
							}
						}
						if (ImGui::IsItemHovered())
						{
							for (auto selected : row_selected)
							{
								selected = false;
							}
							row_selected[table_idx] = true;
						}
						const auto& extension = entry.path().extension().string();
						if (ImGui::BeginPopupContextItem())
						{
							using asset_manager_list = decltype(Engine::asset_manager)::manager_list;
							brigand::for_each<asset_manager_list>([extension, path](auto t)
							{
								using TAssetManager = typename decltype(t)::type;
								using TAsset = typename TAssetManager::AssetType;

								const std::list<std::string>& supported_type = TAsset::supported_formats;

								for (const auto& type : supported_type)
								{
									if (type == extension)
									{
										if (ImGui::SmallButton((std::string{ "Load " } + typeid(TAssetManager).name()).c_str()))
										{
											auto& asset_manager = Engine::asset_manager.GetManager<TAssetManager>();
											asset_manager.LoadAsset(path.string());
											continue;
											ImGui::CloseCurrentPopup();
										}
									}
								}
							});

							if (extension == ".yaml")
							{
								if (ImGui::SmallButton("Load Scene"))
								{
									auto func = &decltype(Engine::scene_serializer)::DeserializeScene;
									auto void_func = std::bind(func, &Engine::scene_serializer, path.string());
									Engine::RegisterEvent(Engine::POST, void_func);
								}
							}

							ImGui::EndPopup();
						}
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Right-click to load");
					}

					ImGui::TableSetColumnIndex(1);
					if (ImGui::Selectable(std::to_string(entry.file_size()).c_str(), row_selected[table_idx]), ImGuiSelectableFlags_AllowDoubleClick)
					{
						if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && ImGui::IsItemHovered())
						{
							if (entry.is_directory())
							{
								directory_path = entry.path().string();
							}
						}
						if (ImGui::IsItemHovered())
						{
							for (auto selected : row_selected)
							{
								selected = false;
							}
							row_selected[table_idx] = true;
						}
					}
					table_idx++;
				}
				ImGui::EndTable();
			}
			ImGui::TextUnformatted(directory_path.c_str());
			if (directory_path != "..\\")
			{
				if (ImGui::SmallButton("..."))
				{
					directory_path = directory_path.substr(0, directory_path.find_last_of('\\'));
				}
			}

			ImGui::InputText("##name", scene_save_name_buf.data, scene_save_name_buf.size);
			ImGui::SameLine();
			if (ImGui::SmallButton("Save"))
			{
				const std::filesystem::path path = (directory_path + "\\").append(scene_save_name_buf.data);
				std::string relative_path = std::filesystem::relative(path, "..\\").string();
				OE::Engine::scene_serializer.SerializeScene(path.string());
			}
			ImGui::End();

			if (open == false)
			{
				Engine::gui_manager.RunBehavior(name);
			}
		}
	}
}
