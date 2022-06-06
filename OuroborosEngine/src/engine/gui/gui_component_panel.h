#pragma once
#include <imgui-docking/imgui.h>
#include "../common.h"
#include "../ecs/ecs_base.h"
#include "../engine.h"
#include "../ecs_settings.h"
#include "engine/ecs_settings.h"
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
					ecs_manager.CreateEntity();
				}
				ImGui::EndPopup();
			}

		ecs_manager.ForEntities([](ecs_ID entID)
			{
				if(ecs_manager.GetEntity(entID).alive)
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
								ecs_manager.DeleteEntity(entID);
							}
							if (ImGui::BeginMenu(popup_menu_strings[COMPONENT_ADD].c_str()))
							{
								using TComponents = decltype(ecs_manager.component_manager)::ComponentList;

								brigand::for_each<TComponents>([entID](auto type)
									{
										using TComponent = typename decltype(type)::type;
										const bool has_this_component = ecs_manager.HasComponent<TComponent>(entID);
										if (has_this_component)
										{
											ImGui::BeginDisabled();
										}
										if (ImGui::MenuItem(typeid(TComponent).name(), nullptr, has_this_component))
										{
											ecs_manager.AddComponent<TComponent>(entID);
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
								using TComponents = decltype(ecs_manager.component_manager)::ComponentList;

								brigand::for_each<TComponents>([entID](auto type)
									{
										using TComponent = typename decltype(type)::type;
										const bool has_this_component = ecs_manager.HasComponent<TComponent>(entID);
										if (!has_this_component)
										{
											ImGui::BeginDisabled();
										}
										if (ImGui::MenuItem(typeid(TComponent).name(), nullptr, has_this_component))
										{
											ecs_manager.DeleteComponent<TComponent>(entID);
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
								using TSystems = decltype(ecs_manager)::SystemStorage::system_list;

								brigand::for_each<TSystems>([entID](auto type)
									{
										using TSystem = typename decltype(type)::type::type;
										const bool has_this_system = ecs_manager.HasSystem<TSystem>(entID);
										if (ImGui::MenuItem(typeid(TSystem).name(), nullptr, has_this_system))
										{
											if(!has_this_system)
											{
												ecs_manager.AddSystem<TSystem>(entID);
											}
											else
											{
												ecs_manager.DeleteSystem<TSystem>(entID);
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
								if (ecs_manager.HasComponent<TComponent>(entID))
								{
									ComponentDrawFunction<TComponent>(entID);
								}
							});
						ImGui::PopID();
					}
				}
			});
	}

	static void EngineInfoPanelFunction()
	{
		ImGui::Text("dt : %f", 1.0);
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
							ecs_manager.ForEntities([](ecs_ID entityID)
								{
									auto& entity = ecs_manager.GetEntity(entityID);
									if (entity.alive)
									{
										if(bool has_system = ecs_manager.HasSystem<T>(entityID))
										{
											using system = T;
											using system_requirement_components = typename system::required_components;
											bool has_all_component = true;
											brigand::for_each<system_requirement_components>([&]<typename T>(T type) mutable
												{
													using component = typename T::type;
													has_all_component &= ecs_manager.HasComponent<component>(entityID);
												});
											ImGui::Bullet();
											ImGui::TextColored(has_all_component ? GUI_Colors::GREEN : GUI_Colors::RED, "%d", entityID);
											if(!has_all_component)
											{
												ImGui::TextColored(GUI_Colors::RED, "\tMissing");
												brigand::for_each<system_requirement_components>([&]<typename T>(T type)
												{
													using component = typename T::type;
													const bool has_component = ecs_manager.HasComponent<component>(entityID);
													if(!has_component)
													{
														using missing_component = component;
														ImGui::TextColored(GUI_Colors::RED, "\t\t%s", typeid(missing_component).name());
														ImGui::SameLine();
														if(ImGui::Button("Add"))
														{
															ecs_manager.AddComponent<missing_component>(entityID);
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
					}
					ImGui::PopID();
				});

			ImGui::EndTabBar();
		}
	}
}

#include "gui_component_implementation.inl"
