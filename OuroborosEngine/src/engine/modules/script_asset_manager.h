#pragma once
#include "master_asset_manager.h"
#include "../src/engine/scripting/script.h"

namespace Asset
{
	struct Script;
}

namespace OE
{
    class ScriptAssetManager : public AssetManager<Asset::Script>
    {
    public:
        virtual int LoadAsset(const std::string& file_name) override;
        virtual int UnloadAsset(const std::string& name) override;

        static OE::Script::ScriptType GetTypeFromExtension(const std::string extension_str)
        {
	        Script::ScriptType type = Script::ScriptType::Normal;
            if (extension_str == ".lua")
            {
                type = Script::ScriptType::Normal;
            }
            else if (extension_str == ".clua")
            {
                type = Script::ScriptType::Component;
            }
            else if (extension_str == ".slua")
            {
                type = Script::ScriptType::System;
            }
            return type;
        }
    };


    inline int ScriptAssetManager::UnloadAsset(const std::string& name)
    {
	    return AssetManager<Asset::Script>::UnloadAsset(name);
    }
}


