#pragma once
#include "master_asset_manager.h"
#include "../common/assets.h"

namespace OE
{
    class ImageAssetManager : public AssetManager<Asset::Image>
    {
    public:
        ~ImageAssetManager() override;
        virtual int LoadAsset(const std::string& name) override;
        //virtual int UnloadAsset(const std::string& name) override;
    };
}
