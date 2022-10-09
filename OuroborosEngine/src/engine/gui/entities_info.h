#pragma once
#include "base.h"
namespace OE
{
	namespace GUI
	{
		class EntitesInfo : public GUI_Base
		{
		public:
			EntitesInfo() : GUI_Base("Entities") {}
			void Draw() override;
		};
	}
}
