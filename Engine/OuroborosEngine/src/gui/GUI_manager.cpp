#include "GUI_manager.h"

#include <ImGuizmo.h>

#include "sample_panel.h"
#include "entities_info.h"
#include "file_browser.h"
#include "asset_browser.h"
#include "system_configure.h"
#include "slider_speed.h"
#include "material_configure.h"
#include "viewport.h"
#include "../core/engine.h"

OE::GUI::_manager_internal::Behavior* OE::GUI::_manager_internal::MenuNode::FindBehavior(
	const std::string& behavior_name, std::queue<std::string>& categories)
{
	const std::string category_name = categories.front();
	categories.pop();
	const bool has_menu_item = child_nodes.contains(category_name);
	if (!has_menu_item)
	{
		return nullptr;
	}
	else
	{
		if(categories.empty())
		{
			if (child_nodes[category_name]->behaviors.contains(behavior_name))
			{
				return &child_nodes[category_name]->behaviors[behavior_name];
			}
			return nullptr;
		}
		return child_nodes[category_name]->FindBehavior(behavior_name, categories);
	}
}

void OE::GUI::_manager_internal::MenuNode::DrawMenu()
{
	for (const auto& [key, node] : child_nodes)
	{
		if (ImGui::BeginMenu(key.c_str()))
		{
			node->DrawMenu();
			ImGui::EndMenu();
		}
	}
	if (child_nodes.empty() == false && behaviors.empty() == false)
	{
		ImGui::Separator();
	}
	for (auto& [key, behavior] : behaviors)
	{
		if(ImGui::MenuItem(key.c_str(), 0, behavior.open))
		{
			behavior();
		}
	}
}

OE::GUI::GUI_manager::GUI_manager() : slider_speed(0.01f)
{
	head_menu = new _manager_internal::MenuNode;
	//std::queue path = { "File", "System" };
	RegisterMenuItem({ "System", "Tests" }, "ImGuiDemo", [this, created = static_cast<GUI_Base*>(nullptr)](bool& open) mutable
	{
		open = !open;
		if(open)
		{
			created = new sample_panel;
			AddPanel(created);
		}
		else
		{
			RemovePanel(created);
		}
	});
	RegisterMenuItem({ "System", "Tests" }, "TestWindow", [this](bool& open)
		{
			//static std::vector<GUI_Base*> bases;
			//GUI_Base* bases_created
			static const std::string base_name = "Sample Window ";
			constexpr int MAX_SAME_ITEMS = 10;
			static bool slots[MAX_SAME_ITEMS] = { false };

			int i = 0;

			for( i = 0; i < MAX_SAME_ITEMS; ++i)
			{
				if(HasPanel(base_name + std::to_string(i)))
				{
					slots[i] = true;
				}
				else
				{
					slots[i] = false;
				}
			}

			for (i = 0; i < MAX_SAME_ITEMS; ++i)
			{
				if(slots[i] == false)
				{
					slots[i] = true;
					break;
				}
			}

			AddPanel(new sample_panel2(base_name + std::to_string(i)));
		});

	RegisterMenuItem({ "ECS" }, "Entities", [this, created = static_cast<GUI_Base*>(nullptr)](bool& open) mutable
	{
		open = !open;
		if (open)
		{
			created = new EntitesInfo;
			AddPanel(created);
		}
		else
		{
			RemovePanel(created);
		}
	});

	RegisterMenuItem({ "ECS" }, "Systems", [this, created = static_cast<GUI_Base*>(nullptr)](bool& open) mutable
	{
		open = !open;
		if (open)
		{
			created = new SystemConfigure;
			AddPanel(created);
		}
		else
		{
			RemovePanel(created);
		}
	});

	RegisterMenuItem({ "File" }, "File browser", [this, created = static_cast<GUI_Base*>(nullptr)](bool& open) mutable
	{
		open = !open;
		if (open)
		{
			created = new FileBrowser;
			AddPanel(created);
		}
		else
		{
			RemovePanel(created);
		}
	});

	RegisterMenuItem({ "File",  "Asset"}, "Asset browser", [this, created = static_cast<GUI_Base*>(nullptr)](bool& open) mutable
	{
		open = !open;
		if (open)
		{
			created = new AssetBrowser;
			AddPanel(created);
		}
		else
		{
			RemovePanel(created);
		}
	});

	RegisterMenuItem({ "File", "Asset" }, "New material", [this](bool& open)
		{
			static const std::string base_name = "New material ";
			constexpr int MAX_SAME_ITEMS = 10;
			static bool slots[MAX_SAME_ITEMS] = { false };

			int i = 0;

			for (i = 0; i < MAX_SAME_ITEMS; ++i)
			{
				if (HasPanel(base_name + std::to_string(i)))
				{
					slots[i] = true;
				}
				else
				{
					slots[i] = false;
				}
			}

			for (i = 0; i < MAX_SAME_ITEMS; ++i)
			{
				if (slots[i] == false)
				{
					slots[i] = true;
					break;
				}
			}

			AddPanel(new MaterialConfigure(base_name + std::to_string(i)));
		});

	RegisterMenuItem({ "System" }, "Slider speed", [this, created = static_cast<GUI_Base*>(nullptr)](bool& open) mutable
	{
		open = !open;
		if (open)
		{
			created = new SliderSpeed;
			AddPanel(created);
		}
		else
		{
			RemovePanel(created);
		}
	});

	RegisterMenuItem({ "System", "Viewport" }, "Enable", [this, created = static_cast<GUI_Base*>(nullptr)](bool& open) mutable
	{
		open = !open;
		if (open)
		{
			created = new ViewPort;
			AddPanel(created);
		}
		else
		{
			RemovePanel(created);
		}
	});

	RegisterMenuItem({ "System", "Viewport" }, "Fit to frame", [this](bool& open) mutable
	{
		open = !open;
	});
}

void OE::GUI::GUI_manager::Update()
{
	//ImGuizmo::BeginFrame();
	DrawMenu();

	static bool opt_fullscreen = true;
	static bool opt_padding = false;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}
	else
	{
		dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
	}

	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
	// and handle the pass-thru hole, so we ask Begin() to not render a background.
	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
	// all active windows docked into it will lose their parent and become undocked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.

	bool open;
	ImGui::Begin("DockSpace Demo", &open, window_flags);
	ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	ImGui::End();

	while (!add_queue_gui_panels.empty())
	{
		auto value_to_add = add_queue_gui_panels.back();
		add_queue_gui_panels.pop();
		gui_panels.push_back(value_to_add);
	}

	for(const auto& panel_class : gui_panels)
	{
		panel_class->Draw();
	}

	while(!remove_queue_gui_panels.empty())
	{
		auto value_to_remove = remove_queue_gui_panels.back();
		remove_queue_gui_panels.pop();

		auto erase_itr = std::ranges::find(gui_panels, value_to_remove);
		if (erase_itr != gui_panels.end())
		{
			gui_panels.erase(erase_itr);
		}
	}
}

void OE::GUI::GUI_manager::RegisterMenuItem(const std::initializer_list<std::string>& categories, const std::string& behavior_name, const std::function<void(bool&)>& behavior) const
{
	//insert to menu;
	std::queue<std::string> path;
	for (const std::string& category : categories)
	{
		path.push(category);
	}
	head_menu->InsertOrCreate(behavior_name, _manager_internal::Behavior(behavior), path);
}

void  OE::GUI::GUI_manager::AddPanel(GUI_Base* panel)
{
	add_queue_gui_panels.push(panel);
}

void OE::GUI::GUI_manager::RemovePanel(GUI_Base* pointer)
{
	auto erase_itr = std::ranges::find(gui_panels, pointer);
	if (erase_itr != gui_panels.end())
	{
		remove_queue_gui_panels.push(pointer);
	}
}

float OE::GUI::GUI_manager::GetSliderSpeed()
{
	return slider_speed;
}

OE::GUI::_manager_internal::Behavior* OE::GUI::GUI_manager::GetBehavior(
	const std::initializer_list<std::string>& categories, const std::string& behavior_name) const
{
	std::queue<std::string> path;
	for (const std::string& category : categories)
	{
		path.push(category);
	}
	//const auto found_behavior = head_menu->FindBehavior(behavior_name, path);
	const auto found_behavior = head_menu->FindBehavior(behavior_name, path);
	return found_behavior;
}

void OE::GUI::GUI_manager::RunBehavior(const std::initializer_list<std::string>& categories,
                                       const std::string& behavior_name) const
{
	if(const auto found_behavior = GetBehavior(categories, behavior_name))
	{
		(*found_behavior)();
	}
	else
	{
		assert(false);
	}
}

void OE::GUI::GUI_manager::DrawMenu()
{
	if(ImGui::BeginMainMenuBar())
	{
		head_menu->DrawMenu();
		ImGui::EndMainMenuBar();
	}
}

bool OE::GUI::GUI_manager::HasPanel(const std::string& panel_name)
{
	const auto itr = std::ranges::find_if(gui_panels, [panel_name](GUI_Base* base)-> bool
	{
		return panel_name == base->GetName();
	});
	return itr != gui_panels.end();
}
