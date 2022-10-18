#pragma once
#include "base.h"
#include "../common.h"
namespace OE
{
	namespace GUI
	{
		class EntitesInfo : public GUI_Base
		{
		public:
			EntitesInfo() : GUI_Base("Entities") {}
			void Draw() override;
		private:
			void DrawSingleEntity(ecs_ID entityID);
			bool	is_popup_menu_opened = false;
			ecs_ID selected_entity = -1;
		};
	}
}
