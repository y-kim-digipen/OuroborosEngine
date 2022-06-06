#pragma once
#include "../common.h"
#include "../ecs/ecs_base.h"
#include "../engine.h"
#include "../ecs_settings.h"
#include <imgui-docking/imgui.h>

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

	inline static void EntityInfoPanelFunction()
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
										//ImGui::Text(typeid(decltype(type)::type).name());
										using TSystem = typename decltype(type)::type::type;
										const bool has_this_system = ecs_manager.HasSystem<TSystem>(entID);
										//if (has_this_system)
										//{
										//	ImGui::BeginDisabled();
										//}
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
										//if (has_this_system)
										//{
										//	ImGui::EndDisabled();
										//}
									});
								ImGui::EndMenu();
							}


							ImGui::EndPopup();
						}

						brigand::for_each<ComponentList>([entID](auto type)
							{
								using TComponent = typename decltype(type)::type;
								//ImGui::Text(typeid(TComponent).name());
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

	template<>
	inline void ComponentDrawFunction<Transform>(ecs_ID entID)
	{
		std::string strID = std::to_string(entID);
		Transform& transform_component = ecs_manager.GetComponent<Transform>(entID);
		if (ImGui::TreeNode(typeid(Transform).name()))
		{
			ImGui::DragFloat3(GET_VARIABLE_NAME(transform_component.pos), &transform_component.pos.x);
			ImGui::DragFloat3(GET_VARIABLE_NAME(transform_component.scale), &transform_component.scale.x);
			ImGui::DragFloat3(GET_VARIABLE_NAME(transform_component.rotateAxis), &transform_component.rotate_axis.x);
			ImGui::DragFloat(GET_VARIABLE_NAME(transform_component.angle), &transform_component.angle);
			ImGui::TreePop();
		}
	}
	
	template<>
	inline void ComponentDrawFunction<Velocity>(ecs_ID entID)
	{
		std::string strID = std::to_string(entID);
		Velocity& velocity_component = ecs_manager.GetComponent<Velocity>(entID);
		if (ImGui::TreeNode(typeid(Velocity).name()))
		{
			ImGui::DragFloat3(GET_VARIABLE_NAME(velocity_component.vel), &velocity_component.vel.x);
			ImGui::TreePop();
		}
	}

	template<>
	inline void ComponentDrawFunction<LifeTime>(ecs_ID entID)
	{
		std::string strID = std::to_string(entID);
		LifeTime& life_time_component = ecs_manager.GetComponent<LifeTime>(entID);
		if (ImGui::TreeNode(typeid(LifeTime).name()))
		{
			ImGui::DragFloat(GET_VARIABLE_NAME(life_time_component.life_time), &life_time_component.life_time);
			ImGui::TreePop();
		}
	}

	template<>
	inline void ComponentDrawFunction<Tag>(ecs_ID entID)
	{
		std::string strID = std::to_string(entID);
		Tag& tag_component = ecs_manager.GetComponent<Tag>(entID);
		if (ImGui::TreeNode(typeid(Tag).name()))
		{
			constexpr int BufSize = 256;
			static char buf[BufSize];
			//std::vector<char> buf(tag_component.tag.length());
			// ReSharper disable once CppDeprecatedEntity
			strcpy_s(buf, tag_component.tag.c_str());

			if(ImGui::InputText(GET_VARIABLE_NAME(life_time_component.life_time), buf, BufSize))
			{
				tag_component.tag = buf;
			}

			ImGui::TreePop();
		}
	}
}

