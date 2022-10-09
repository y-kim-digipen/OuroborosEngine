#pragma once
#include "base.h"
namespace OE
{
	namespace GUI
	{
		class SystemConfigure : public GUI_Base
		{
		public:
			SystemConfigure() : GUI_Base("System configure") {}
			void Draw() override;
		private:

		};
	}
}
