#include "script_asset_manager.h"

#include <filesystem>

#include "../src/engine/engine.h"
#include "script_asset_manager.h"
#include <fstream>
namespace OE
{
    int ScriptAssetManager::LoadAsset(const std::string& file_name)
    {
        using ScriptType = OE::Script::ScriptType;

        std::filesystem::path script_path{file_name};
        const auto script_extension = script_path.extension().string();
        ScriptType type = ScriptType::Normal;
        if(script_extension == ".lua")
        {
            type = ScriptType::Normal;
        }
        else if (script_extension == ".clua")
        {
            type = ScriptType::Component;
        }
        else if (script_extension == ".slua")
        {
            type = ScriptType::System;
        }
        else
        {
            assert(false);
        }

        auto script = OE::Engine::lua_script_manager.CreateScript(file_name, type, file_name);
        bool res = script != nullptr || script->RunScript();
        if (res == false)
        {
            std::cerr << script->GetLastError() << std::endl;
        }

        assets[file_name] = std::pair(res, Asset::Script(file_name));

        return 0;
    }
}