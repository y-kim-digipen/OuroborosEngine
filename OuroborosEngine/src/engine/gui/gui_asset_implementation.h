#pragma once
#include "../src/engine/gui/GUI_definedColors.h"
#include <fstream>
#include <string>
#include "../common/assets.h"
#include "engine.h"

namespace OE
{
	template<typename TAsset>
	void AssetImGuiImpl(const TAsset& asset)
	{
		ImGui::TextColored(OE::GUI_Colors::RED, "Implementation for this asset is not exist");
	}

	template<>
	inline void AssetImGuiImpl<Asset::Image>(const Asset::Image& image)
	{
		ImGui::TextWrapped("Width : %d", image.width); ImGui::SameLine();
		ImGui::TextWrapped("Height : %d", image.height);
	}

	template<>
	inline void AssetImGuiImpl<Asset::Script>(const Asset::Script& script)
	{
		const std::filesystem::path script_path(script.path);
		const std::string& extension = script_path.extension().string();

		std::ifstream script_file;

		script_file.open(script_path);

		if (script_file.is_open())
		{
			std::string line;
			while (std::getline(script_file, line))
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
			ImGui::TextColored(OE::GUI_Colors::RED, "Cannot open file %s", script_path.c_str());
		}

		if (ImGui::SmallButton("Reload changes"))
		{
			const auto pScript = OE::Engine::lua_script_manager.GetScript(ScriptAssetManager::GetTypeFromExtension(extension), script_path.string());
			pScript->ChangeScript(script_path.string());
		}
		//ImGui::TextWrapped(.c_str());
	}
}
