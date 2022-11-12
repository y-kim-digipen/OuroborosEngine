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

			static std::pair<uint16_t, uint16_t> GetViewPortPos();
			static std::pair<uint16_t, uint16_t> GetViewPortSize();
			static void SetTargetRenderSize(uint16_t width, uint16_t height);
		private:
			inline static uint16_t target_render_width = 1600, target_render_height = 900;
			inline static float target_aspect_ratio = 1.f;
			inline static uint16_t item_pos_x = 0, item_pos_y = 0;
			inline static uint16_t item_width_x = 0, item_height_y = 0;
		};
	}
}
