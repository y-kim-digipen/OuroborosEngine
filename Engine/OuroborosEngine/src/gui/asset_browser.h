#pragma once
#include "base.h"

namespace OE
{
	namespace GUI
	{
		class AssetBrowser : public GUI_Base
		{
		public:
			AssetBrowser() : GUI_Base("Asset browser") {}
			void Draw() override;
		};
	}
}
