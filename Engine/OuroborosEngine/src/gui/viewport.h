#pragma once
#include "base.h"

namespace OE
{
	namespace GUI
	{
		class ViewPort : public GUI_Base
		{
		public:
			ViewPort() : GUI_Base("Viewport") {}
			void Draw() override;
		public:
			void draw_internal();
			void draw_gizmo();
			std::pair<int, int> GetViewPortPos() const;
			std::pair<int, int> GetViewPortSize() const;
		private:
			int item_pos_x = 0, item_pos_y = 0;
			int item_width_x = 0, item_height_y = 0;
		};
	}
}
