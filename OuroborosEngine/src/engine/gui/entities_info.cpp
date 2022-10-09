#include "entities_info.h"
#include "../engine.h"
#include "components_draw.h"

static std::vector<std::string> popup_menu_strings =
{ "Add Component", "Add Entity", "Delete Entity", "Delete Component", "System" };

enum PopupMenuTypes
{
	COMPONENT_ADD, ENTITY_ADD, ENTITY_DELETE, COMPONENT_DELETE, SYSTEM
};

void OE::GUI::EntitesInfo::Draw()
{
	ImGui::Begin(name.c_str(), &open);
	static bool is_hovered = false;
	static bool is_popup_menu_opened = false;
	static int last_ent_hovered = -1;
	auto& engine = Engine::Get();
	using ComponentList = decltype(std::declval<ECS_Manager>().component_manager)::ComponentList;

	Engine::ecs_manager.ForEntities([](ecs_ID entID) mutable
		{
			if (Engine::ecs_manager.GetEntity(entID).alive)
			{
				std::string strID = std::to_string(entID);
				bool collapsing_header_opened = ImGui::CollapsingHeader(strID.c_str());
				if (collapsing_header_opened)
				{
					if (is_popup_menu_opened == false)
					{
						bool is_this_ent_hovered = ImGui::IsItemHovered();
						is_hovered |= is_this_ent_hovered;
						if (is_this_ent_hovered)
						{
							last_ent_hovered = entID;
						}
					}

					ImGui::PushID(entID);
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
				else
				{
					if (is_popup_menu_opened == false)
					{
						bool is_this_ent_hovered = ImGui::IsItemHovered();
						is_hovered |= is_this_ent_hovered;
						if (is_this_ent_hovered)
						{
							last_ent_hovered = entID;
						}
					}
				}
			}
		});
	is_popup_menu_opened = ImGui::BeginPopupContextWindow();
	if (is_popup_menu_opened)
	{
		if (!is_hovered)
		{
			if (ImGui::MenuItem(popup_menu_strings[ENTITY_ADD].c_str()))
			{
				Engine::ecs_manager.CreateEntity();
			}
		}

		if (last_ent_hovered != -1)
		{
			ecs_ID entID = last_ent_hovered;
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
							if (!has_this_system)
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

		}

		ImGui::EndPopup();
	}
	else
	{
		last_ent_hovered = -1;
		is_hovered = false;
	}
	ImGui::End();
}
