#include "script_asset_manager.h"

#include <filesystem>

#include "../src/engine/engine.h"
#include <fstream>
namespace OE
{
    int ScriptAssetManager::LoadAsset(const std::string& file_name)
    {
        std::filesystem::path script_path{file_name};
        const auto script_extension = script_path.extension().string();
        Script::Type type = Script::Type::Normal;
        if(script_extension == ".lua")
        {
            type = Script::Type::Normal;
        }
        else if (script_extension == ".clua")
        {
            type = Script::Type::Component;
        }
        else if (script_extension == ".slua")
        {
            type = Script::Type::System;
        }
        else
        {
            assert(false);

        }

    	auto& script = Engine::lua_script_manager.CreateScript(file_name, type);

        auto func = script.GetFunction("Init");

        bool res = func.valid();

        return 0;
    }
}