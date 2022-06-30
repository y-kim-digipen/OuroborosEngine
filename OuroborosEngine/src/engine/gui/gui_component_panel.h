#pragma once
#include <imgui-docking/imgui.h>

#include <filesystem>

#include "../common.h"
#include "../ecs/ecs_base.h"
#include "../engine.h"
#include "../engine_settings.h"
#include "../common/assets.h"
#include "GUI_definedColors.h"

namespace _imgui_helper
{
	inline bool AABB(ImVec2 p, ImVec2 min, ImVec2 max) {
		return (p.x >= min.x && p.x <= max.x) &&
			(p.y >= min.y && p.y <= max.y);
	}
}
namespace OE
{
	static std::vector<std::string> popup_menu_strings = 
	{ "Add Component", "Add Entity", "Delete Entity", "Delete Component", "System"};

	enum PopupMenuTypes
	{
		COMPONENT_ADD, ENTITY_ADD, ENTITY_DELETE, COMPONENT_DELETE, SYSTEM
	};

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

	static void FileSystemPanelFunction();

	template<typename TComponent>
	void ComponentDrawFunction(ecs_ID entID)
	{
		ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Component %s detail is not implemented", typeid(TComponent).name());
	}

	static void EntityInfoPanelFunction()
	{
		auto& engine = Engine::Get();
		using ComponentList = decltype(std::declval<ECS_Manager>().component_manager)::ComponentList;

			if (!ImGui::IsPopupOpen(popup_menu_strings[COMPONENT_ADD].c_str()) 
				&& !ImGui::IsPopupOpen(popup_menu_strings[COMPONENT_DELETE].c_str())
				&& ImGui::BeginPopupContextWindow())
			{
				if (ImGui::MenuItem(popup_menu_strings[ENTITY_ADD].c_str()))
				{
					Engine::ecs_manager.CreateEntity();
				}
				ImGui::EndPopup();
			}

		Engine::ecs_manager.ForEntities([](ecs_ID entID)
			{
				if(Engine::ecs_manager.GetEntity(entID).alive)
				{
					std::string strID = std::to_string(entID);
					if (ImGui::CollapsingHeader(strID.c_str()))
					{
						//auto item_min		= ImGui::GetItemRectMin();
						//auto item_max		= ImGui::GetItemRectMax();
						//auto cursor_pos		= ImGui::GetMousePosOnOpeningCurrentPopup();
						//bool on_this_item = _imgui_helper::AABB(cursor_pos, item_min, item_max);

						ImGui::PushID(entID);
						if (ImGui::BeginPopupContextWindow(popup_menu_strings[COMPONENT_ADD].c_str()))
						{
							//std::string menu_str = "Add Component to"
							if (ImGui::MenuItem(popup_menu_strings[ENTITY_DELETE].c_str()))
							{
								Engine::ecs_manager.DeleteEntity(entID);
							}
							if (ImGui::BeginMenu(popup_menu_strings[COMPONENT_ADD].c_str()))
							{
								using TComponents = decltype(Engine::ecs_manager.component_manager)::ComponentList;

								brigand::for_each<TComponents>([entID](auto type)
									{
										using TComponent = typename decltype(type)::type;
										const bool has_this_component = OE::Engine::ecs_manager.HasComponent<TComponent>(entID);
										if (has_this_component)
										{
											ImGui::BeginDisabled();
										}
										if (ImGui::MenuItem(typeid(TComponent).name(), nullptr, has_this_component))
										{
											OE::Engine::ecs_manager.AddComponent<TComponent>(entID);
										}
										if (has_this_component)
										{
											ImGui::EndDisabled();
										}
									});
								ImGui::EndMenu();
							}
							if (ImGui::BeginMenu(popup_menu_strings[COMPONENT_DELETE].c_str()))
							{
								using TComponents = decltype(Engine::ecs_manager.component_manager)::ComponentList;

								brigand::for_each<TComponents>([entID](auto type)
									{
										using TComponent = typename decltype(type)::type;
										const bool has_this_component = OE::Engine::ecs_manager.HasComponent<TComponent>(entID);
										if (!has_this_component)
										{
											ImGui::BeginDisabled();
										}
										if (ImGui::MenuItem(typeid(TComponent).name(), nullptr, has_this_component))
										{
											OE::Engine::ecs_manager.DeleteComponent<TComponent>(entID);
										}
										if (!has_this_component)
										{
											ImGui::EndDisabled();
										}
									});

								ImGui::EndMenu();
							}

							if (ImGui::BeginMenu(popup_menu_strings[SYSTEM].c_str()))
							{
								using TSystems = decltype(Engine::ecs_manager)::SystemStorage::system_list;

								brigand::for_each<TSystems>([entID](auto type)
									{
										using TSystem = typename decltype(type)::type::type;
										const bool has_this_system = OE::Engine::ecs_manager.HasSystem<TSystem>(entID);
										if (ImGui::MenuItem(typeid(TSystem).name(), nullptr, has_this_system))
										{
											if(!has_this_system)
											{
												OE::Engine::ecs_manager.AddSystem<TSystem>(entID);
											}
											else
											{
												OE::Engine::ecs_manager.DeleteSystem<TSystem>(entID);
											}
										}
									});
								ImGui::EndMenu();
							}
							ImGui::EndPopup();
						}

						brigand::for_each<ComponentList>([entID](auto type)
							{
								using TComponent = typename decltype(type)::type;
								if (OE::Engine::ecs_manager.HasComponent<TComponent>(entID))
								{
									ComponentDrawFunction<TComponent>(entID);
								}
							});
						ImGui::PopID();
					}
				}
			});
	}

	static void AssetBrowserPanelFunction()
	{
		using manager_list = decltype(std::declval<Engine>().asset_manager)::manager_list;

		if (ImGui::BeginTabBar(""))
		{
			brigand::for_each<manager_list>([](auto type)
				{
					using T = typename decltype(type)::type;
					auto& manager = Engine::Get().asset_manager.GetManager<T>();
					const std::string typeID_name = std::string(typeid(T).name());
					const std::string tab_name = typeID_name.substr(typeID_name.find_last_of("::") + 1);
					const auto& raw_elements_map = manager.GetAssetRawData();
					if(ImGui::BeginTabItem(tab_name.c_str()))
					{
						for (const auto& [name, asset]: raw_elements_map)
						{
							if(ImGui::CollapsingHeader(name.c_str()))
							{
								OE::ImGuiImpl(asset);
							}
						}

						ImGui::EndTabItem();
					}
				});
			ImGui::EndTabBar();
		}

		if(ImGui::SmallButton("Load"))
		{
			//ImGui::Begin("Load file");
			FileSystemPanelFunction();
			//ImGui::End();
		}
	}

	static void EngineInfoPanelFunction()
	{
		ImGui::Text("dt : %f", Engine::delta_timer.GetDeltaTime());
		ImGui::Text("FPS : %d", (int)(1.0 / Engine::delta_timer.GetDeltaTime()));
	}

	static void FileSystemPanelFunction()
	{
		static std::string directory_path = "..\\";

		static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;
		static std::vector<bool> row_selected(50);
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
						//if(ImGui::MenuItem("Load"))
						//{
							using asset_manager_list = decltype(Engine::asset_manager)::manager_list;
							brigand::for_each<asset_manager_list>([extension, path](auto t)
								{
									using TAssetManager = typename decltype(t)::type;
									using TAsset = typename TAssetManager::asset_type;

									const std::list<std::string>& supported_type = TAsset::supported_formats;

									for (const auto& type : supported_type)
									{
										if(type == extension)
										{
											//ImGui::MEnu
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
							//ImGui::CloseCurrentPopup();
						//}
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
	}

	static void SystemInfoPanelFunction()
	{
		using system_list = ECS_Manager::SystemStorage::system_list;
		using component_list = ECS_Manager::ComponentStorage;

		if (ImGui::BeginTabBar("Systems"))
		{
			brigand::for_each<system_list>([&](auto type)
				{
					using T = typename decltype(type)::type;
					const auto strID = typeid(T).name();
					ImGui::PushID(ImGui::GetID(strID));
					if(ImGui::BeginTabItem(strID))
					{
						if(ImGui::BeginListBox(""))
						{
							OE::Engine::ecs_manager.ForEntities([](ecs_ID entityID)
								{
									auto& entity = OE::Engine::ecs_manager.GetEntity(entityID);
									if (entity.alive)
									{
										if(bool has_system = OE::Engine::ecs_manager.HasSystem<T>(entityID))
										{
											using system = T;
											using system_requirement_components = typename system::required_components;
											bool has_all_component = true;
											brigand::for_each<system_requirement_components>([&]<typename T>(T type) mutable
												{
													using component = typename T::type;
													has_all_component &= OE::Engine::ecs_manager.HasComponent<component>(entityID);
												});
											ImGui::Bullet();
											ImGui::TextColored(has_all_component ? GUI_Colors::GREEN : GUI_Colors::RED, "%d", entityID);
											if(!has_all_component)
											{
												ImGui::TextColored(GUI_Colors::RED, "\tMissing");
												brigand::for_each<system_requirement_components>([&]<typename T>(T type)
												{
													using component = typename T::type;
													const bool has_component = OE::Engine::ecs_manager.HasComponent<component>(entityID);
													if(!has_component)
													{
														using missing_component = component;
														ImGui::TextColored(GUI_Colors::RED, "\t\t%s", typeid(missing_component).name());
														ImGui::SameLine();
														if(ImGui::Button("Add"))
														{
															OE::Engine::ecs_manager.AddComponent<missing_component>(entityID);
														}
													}
												});
											}
										}
									}
								});
							ImGui::EndListBox();
						}
						ImGui::EndTabItem();


						const bool has_native_implementation = OE::Engine::ecs_manager.system_storage.HasSystemImplementation<T>();
						const bool has_script_implementation = OE::Engine::ecs_manager.system_storage.HasSystemScript<T>();
						const auto system_current_usage = OE::Engine::ecs_manager.system_storage.GetSystemUsage<T>();
						using ECS_Manager = decltype(OE::Engine::ecs_manager);
						using system_usage_type = ECS_Manager::settings::system_storage::system_usage_type;

						bool using_native = system_current_usage == system_usage_type::NATIVE;

						if (ImGui::Checkbox("Native", &using_native))
						{
							if (has_native_implementation)
							{
								OE::Engine::ecs_manager.system_storage.SetSystemUsage<T>(system_usage_type::NATIVE);
							}
						}
						ImGui::SameLine();
						has_native_implementation ? (ImGui::TextColored(GUI_Colors::GREEN, "System has native implementation")) : (ImGui::TextColored(GUI_Colors::RED, "System doesn't have native implementation"));


						bool using_lua = system_current_usage == system_usage_type::SCRIPT;
						if (ImGui::Checkbox("LuaScript", &using_lua))
						{
							if (has_script_implementation)
							{
								OE::Engine::ecs_manager.system_storage.SetSystemUsage<T>(system_usage_type::SCRIPT);
							}
						}
						ImGui::SameLine();
						has_script_implementation ? (ImGui::TextColored(GUI_Colors::GREEN, "System has script implementation")) : (ImGui::TextColored(GUI_Colors::RED, "System doesn't have script implementation"));


						auto& system_scripts = OE::Engine::lua_script_manager.scripts[Script::Type::System];
						std::string script_using = OE::Engine::ecs_manager.system_storage.GetSystemScript<T>();
						if(ImGui::BeginCombo("Scripts", script_using.c_str()))
						{
							for (Script& script : system_scripts)
							{
								const std::string& script_path = script.script_path;
								const bool selected = script_using == script_path;
								if(ImGui::Selectable(script_path.c_str(), selected))
								{
									auto init_func = script.GetFunction("Init");

									OE::Engine::ecs_manager.system_storage.RegisterSystemScript<T>(script_path);
									OE::Engine::ecs_manager.ForEntitiesMatching<T>(0.0f, init_func);
								}
							}
							ImGui::EndCombo();
						}
					}
					ImGui::PopID();
				});

			ImGui::EndTabBar();
		}
	}
}

#include "gui_component_implementation.inl"
