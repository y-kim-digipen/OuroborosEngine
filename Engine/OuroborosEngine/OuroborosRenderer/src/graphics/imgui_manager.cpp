#include "imgui_manager.h"

namespace Renderer
{
	void ImguiManager::Update()
	{			//const ImGuiViewport* viewport = ImGui::GetMainViewport();
			//ImGui::SetNextWindowPos(viewport->WorkPos);
			//ImGui::SetNextWindowSize(viewport->WorkSize);
			//ImGui::SetNextWindowViewport(viewport->ID);
			//ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			//ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			//ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

			//ImGuiWindowFlags window_flags =/* ImGuiWindowFlags_MenuBar | */ImGuiWindowFlags_NoDocking;

			//window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			//window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

			//ImGui::Begin("DockSpace", nullptr, window_flags);
			//ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");

			//ImGui::DockSpace(dockspace_id, ImVec2(), ImGuiDockNodeFlags_None);
			//ImGui::PopStyleVar();
			//ImGui::PopStyleVar();
			//ImGui::PopStyleVar();
			//ImGui::End();

		//if (ImGui::BeginMainMenuBar())
		//{
		//	for (auto& [menu_name, map] : panel_functions)
		//	{
		//		if (ImGui::BeginMenu(menu_name.c_str()))
		//		{

		//			for (auto& [key, value] : map)
		//			{
		//				auto& open = value.second;
		//				if (ImGui::MenuItem(key.c_str(), nullptr, open))
		//				{
		//					open = !open;
		//				}
		//			}
		//			ImGui::EndMenu();
		//		}
		//	}
		//	ImGui::EndMainMenuBar();
		//}

		//for (auto& menu : panel_functions | std::views::values)
		//{
		//	for (auto& [key, pair] : menu)
		//	{
		//		auto& [function, open] = pair;
		//		if (open)
		//		{
		//			//ImGui::Begin(key.c_str(), &open);
		//			function(key, &open);
		//			//ImGui::End();
		//		}
		//	}
		//}

		//for(auto& panel_class : panel_classes)
		//{
		//	bool place_holder = true;
		//	panel_class.second->Draw(panel_class.first, &place_holder);
		//}
	}
}
