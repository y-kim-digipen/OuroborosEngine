#include "base.h"
#include "../core/engine.h"
#include "GUI_manager.h"

namespace OE
{
	void GUI::GUI_Base::RemovePanel()
	{
		OE::Engine::gui_manager.RemovePanel(this);
	}
}



