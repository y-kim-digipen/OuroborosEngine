#pragma once
#include "base.h"

namespace OE
{
	namespace GUI
	{
		class FileBrowser : public GUI_Base
		{
		public:
			FileBrowser() : GUI_Base("File browser") {}
			void Draw() override;
		private:
			DefaultSizeBuffer scene_save_name_buf{ "" };
		};
	}
}
