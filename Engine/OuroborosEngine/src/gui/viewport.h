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
		};
	}
}
