#ifndef IMGUIMANAGER_H
#define IMGUIMANAGER_H
#include <functional>
#include <map>
#include <ranges>
#include <string>
#include <GLFW/glfw3.h>
#include <imgui-docking/imgui.h>

namespace Renderer
{
	class ImguiManager
	{
	public:
		using GUI_PanelFunction = std::function<void()>;
	public:
		ImguiManager() = default;
		virtual void Init(GLFWwindow* window) = 0;
		virtual void Update()
		{
			//const ImGuiViewport* viewport = ImGui::GetMainViewport();
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
			
			if (ImGui::BeginMainMenuBar())
			{
				for (auto& [menu_name, map] : panel_functions)
				{
					if (ImGui::BeginMenu(menu_name.c_str()))
					{

						for (auto& [key, value] : map)
						{
							auto& open = value.second;
							if (ImGui::MenuItem(key.c_str(), nullptr, open))
							{
								open = !open;
							}
						}
						ImGui::EndMenu();
					}
				}
				ImGui::EndMainMenuBar();
			}

			for (auto& menu : panel_functions | std::views::values)
			{
				for (auto& [key, pair] : menu)
				{
					auto& [function, open] = pair;
					if (open)
					{
						ImGui::Begin(key.c_str(), &open);
						function();
						ImGui::End();
					}
				}
			}
		}

		virtual void Shutdown() =0;
		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;

		int RegisterPanel(std::string&& menu_name, std::string&& key, GUI_PanelFunction&& function, bool wake_on_open = true)
		{
			auto& menu = panel_functions[menu_name];
			return menu.try_emplace(std::move(key), std::pair(std::move(function), wake_on_open)).second == true;
		}

		auto& GetPanels()
		{
			return panel_functions;
		}

	protected:
		GLFWwindow* window_ = nullptr;
		std::unordered_map<std::string, std::unordered_map<std::string, std::pair<GUI_PanelFunction, bool>>> panel_functions;
	};
}





#endif