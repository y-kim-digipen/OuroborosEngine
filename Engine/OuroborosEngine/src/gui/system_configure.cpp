#include "system_configure.h"
#include "../core/engine.h"
namespace OE
{
	namespace GUI
	{
		void SystemConfigure::Draw()
		{
			ImGui::Begin(name.c_str(), &open);
			using system_list = ECS_Manager::SystemStorage::system_list;
			using component_list = ECS_Manager::ComponentStorage;

			if (ImGui::BeginTabBar("Systems"))
			{
				brigand::for_each<system_list>([&](auto type)
					{
						using T = typename decltype(type)::type;
						const auto strID = typeid(T).name();
						ImGui::PushID(ImGui::GetID(strID));
						if (ImGui::BeginTabItem(strID))
						{
							if (ImGui::BeginListBox(""))
							{
								OE::Engine::ecs_manager.ForEntities([](ecs_ID entityID)
									{
										auto& entity = OE::Engine::ecs_manager.GetEntity(entityID);
										if (entity.alive)
										{
											if (bool has_system = OE::Engine::ecs_manager.HasSystem<T>(entityID))
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
												ImGui::TextColored(has_all_component ? GUI::Colors::GREEN : GUI::Colors::RED, "%d", entityID);
												if (!has_all_component)
												{
													ImGui::TextColored(GUI::Colors::RED, "\tMissing");
													brigand::for_each<system_requirement_components>([&]<typename T>(T type)
													{
														using component = typename T::type;
														const bool has_component = OE::Engine::ecs_manager.HasComponent<component>(entityID);
														if (!has_component)
														{
															using missing_component = component;
															ImGui::TextColored(GUI::Colors::RED, "\t\t%s", typeid(missing_component).name());
															ImGui::SameLine();
															if (ImGui::Button("Add"))
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
							has_native_implementation ? (ImGui::TextColored(GUI::Colors::GREEN, "System has native implementation")) : (ImGui::TextColored(GUI::Colors::RED, "System doesn't have native implementation"));


							bool using_lua = system_current_usage == system_usage_type::SCRIPT;
							if (ImGui::Checkbox("LuaScript", &using_lua))
							{
								if (has_script_implementation)
								{
									OE::Engine::ecs_manager.system_storage.SetSystemUsage<T>(system_usage_type::SCRIPT);
								}
							}
							ImGui::SameLine();
							has_script_implementation ? (ImGui::TextColored(GUI::Colors::GREEN, "System has script implementation")) : (ImGui::TextColored(GUI::Colors::RED, "System doesn't have script implementation"));


							auto& system_scripts = OE::Engine::lua_script_manager.GetScripts(Script::ScriptType::System);
							std::string script_using = OE::Engine::ecs_manager.system_storage.GetSystemScript<T>();
							if (ImGui::BeginCombo("Scripts", script_using.c_str()))
							{
								for (auto& [name, script] : system_scripts)
								{
									const std::string& script_path = script.script_path;
									const bool selected = script_using == script_path;
									if (ImGui::Selectable(script_path.c_str(), selected))
									{
										//auto init_func = script.GetFunction("Init");

										OE::Engine::ecs_manager.system_storage.RegisterSystemScript<T>(script_path);
										//OE::Engine::ecs_manager.ForEntitiesMatching<T>(0.0f, init_func);
									}
								}
								ImGui::EndCombo();
							}
						}
						ImGui::PopID();
					});

				ImGui::EndTabBar();
			}
			ImGui::End();

			if (open == false)
			{
				Engine::gui_manager.RunBehavior("Systems");
			}
		}
	}
}
