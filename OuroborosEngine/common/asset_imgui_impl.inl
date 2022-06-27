#pragma once
#include "../src/engine/gui/GUI_definedColors.h"

namespace OE
{
	template<typename TAsset>
	inline void ImGuiImpl(const TAsset& asset)
	{
		ImGui::TextColored(OE::GUI_Colors::RED, "Implementation for this asset is not exist");
	};

	template<>
	inline void ImGuiImpl(const Asset::Image& image)
	{
		ImGui::TextWrapped("Width : %d", image.width); ImGui::SameLine();
		ImGui::TextWrapped("Height : %d", image.height);
		
	}




	
}
