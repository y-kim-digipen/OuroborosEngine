#pragma once
#include "base.h"
namespace OE
{
	namespace GUI
	{
		class SliderSpeed : public GUI_Base
		{
		public:
			SliderSpeed() : GUI_Base("Slider speed"){}
			void Draw() override;


		};
	}
}