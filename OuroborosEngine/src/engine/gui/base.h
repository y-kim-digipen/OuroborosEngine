#pragma once
#include "gui_definitions.h"
namespace OE
{
	namespace GUI
	{
		namespace _manager_internal
		{
			struct MenuNode;
		}

		class GUI_Base
		{
			friend struct _manager_internal::MenuNode;
		public:
			GUI_Base(const std::string& name, PanelFlag flag = PanelFlags::NONE) : name{ name }, flag{ flag }, open{ true } {}
			virtual void Draw() = 0;

			const std::string& GetName() const
			{
				return name;
			}

			PanelFlag GetFlag() const
			{
				return flag;
			}

			bool IsOpen() const
			{
				return open;
			}
		protected:
			std::string name;
			PanelFlag flag;
			bool open;
		};
	}
}