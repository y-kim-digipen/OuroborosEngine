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
        if(script == nullptr)
        {
            std::cerr << "Failed to load script" << file_name << std::endl;
            return 1;
        }
        bool res = script->RunScript();
        if (res == false)
        {
            std::cerr << script->GetLastError() << std::endl;
        }

        Asset::Script script_asset;
        script_asset.path = file_name;

        assets[file_name] = std::pair(res, script_asset);

        return 0;
    }

    int ScriptAssetManager::UnloadAsset(const std::string& name)
    {
        OE::Engine::lua_script_manager.DeleteScript(name);
    	return AssetManager<Asset::Script>::UnloadAsset(name);
        
    }

    void ScriptAssetManager::CleanUp()
    {
	    for (auto asset : assets)
	    {
            OE::Engine::lua_script_manager.DeleteScript(asset.first);
	    }
	    AssetManager<Asset::Script>::CleanUp();
    }
}
