#pragma once
#include "../src/engine/gui/GUI_definedColors.h"
#include <fstream>
#include <iostream>
#include <string>

namespace OE
{
	template<typename TAsset>
	void ImGuiImpl(const TAsset& asset)
	{
		ImGui::TextColored(OE::GUI_Colors::RED, "Implementation for this asset is not exist");
	}

	template<>
	inline void ImGuiImpl<Asset::Image>(const Asset::Image& image)
	{
		ImGui::TextWrapped("Width : %d", image.width); ImGui::SameLine();
		ImGui::TextWrapped("Height : %d", image.height);
		
	}

	template<>
	inline void ImGuiImpl<Asset::Script>(const Asset::Script& script)
	{
		const std::string& file_path = script.path;

		std::ifstream script_file;
		script_file.open(file_path);

		if(script_file.is_open())
		{
			std::string line;
			while(std::getline(script_file, line))
			{
				if (line.contains("--"))
				{
					ImGui::TextColored(OE::GUI_Colors::GREEN, line.c_str());
				}

				else
				{
					ImGui::Text(line.c_str());
				}
				
			}
		}
		else
		{
			ImGui::TextColored(OE::GUI_Colors::RED, "Cannot open file %s", file_path.c_str());
		}

		if(ImGui::SmallButton("Reload changes"))
		{
			/*Engine::*/
		}
		//ImGui::TextWrapped(.c_str());
	}
}
