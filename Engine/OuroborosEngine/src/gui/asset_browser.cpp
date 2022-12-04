#include "asset_browser.h"
#include "../core/engine.h"
#include "asset_draw.h"

namespace OE
{
	namespace GUI
	{
		void AssetBrowser::Draw()
		{
			using manager_list = decltype(std::declval<Engine>().asset_manager)::manager_list;
			ImGui::Begin(name.c_str(), &open);
			if (ImGui::BeginTabBar(""))
			{
				brigand::for_each<manager_list>([](auto type)
					{
						using T = typename decltype(type)::type;
						auto& manager = Engine::Get().asset_manager.GetManager<T>();
						const std::string typeID_name = std::string(typeid(T).name());
						const std::string tab_name = typeID_name.substr(typeID_name.find_last_of("::") + 1);
						const auto& raw_elements_map = manager.GetAssetRawData();
						if (ImGui::BeginTabItem(tab_name.c_str()))
						{
							for (const auto& [name, asset] : raw_elements_map)
							{
								if (ImGui::CollapsingHeader(name.c_str()))
								{
									bool is_asset_ready = asset.first;
									if (!is_asset_ready)
									{
										ImGui::TextColored(OE::GUI::Colors::RED, "Asset is not ready for use");
									}
									else
									{
										OE::AssetImGuiImpl(asset.second);
									}
								}
							}
							ImGui::EndTabItem();
						}
					});
				ImGui::EndTabBar();
			}

			//if(ImGui::SmallButton("Load"))
			//{
			//	Engine::GetRenderWindow()->vulkan_imgui_manager.GetPanels()["System"]["AssetManager"].second = true;
			//}
			ImGui::End();

			if (open == false)
			{
				Engine::gui_manager.RunBehavior(name);
			}
		}
	}
}
