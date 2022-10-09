#pragma once
#include "base.h"

namespace OE
{
	namespace GUI
	{
		class ViewPort : public GUI_Base
		{
		public:
			ViewPort() : GUI_Base("View port") {}
			void Draw() override;
		public:
			void draw_internal();
		};
	}
}
