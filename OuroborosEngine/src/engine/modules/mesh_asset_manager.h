#pragma once
#include <iostream>

#include "master_asset_manager.h"
#include "../common/assets.h"


namespace OE
{
    class MeshAssetManager : public AssetManager<Asset::Mesh>
    {
    public:
        virtual int LoadAsset(const std::string& file_name) override;
        virtual int UnloadAsset(const std::string& name) override;
        virtual void CleanUp() override;

    };
}