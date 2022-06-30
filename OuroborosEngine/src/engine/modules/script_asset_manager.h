#pragma once
#include "master_asset_manager.h"

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
    };



    inline int ScriptAssetManager::UnloadAsset(const std::string& name)
    {
	    return AssetManager<Asset::Script>::UnloadAsset(name);
    }
}


