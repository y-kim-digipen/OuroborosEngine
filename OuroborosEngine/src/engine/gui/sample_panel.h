#pragma once
#include "base.h"
#include "../engine.h"
#include <imgui-docking/imgui.h>
namespace OE
{
	namespace GUI
	{
		class sample_panel :
			public OE::GUI::GUI_Base
		{
		public:
			sample_panel() : GUI_Base("Sample Panel", PanelFlags::EXIST_ONE) {}
			void Draw() override
			{
				ImGui::ShowDemoWindow(&open);
			}
		};

		class sample_panel2 :
			public OE::GUI::GUI_Base
		{
		public:
			sample_panel2(const std::string& name) : GUI_Base(name, PanelFlags::EXIST_ONE) { open = true; }
			void Draw() override
			{
				ImGui::Begin(name.c_str(), &open);
				{
					
				}
				ImGui::End();
				if(open == false)
				{
					Engine::gui_manager.RemovePanel(this);
				}
			}
		};
	}
}


