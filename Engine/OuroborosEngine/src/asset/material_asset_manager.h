#pragma once
#include <common/assets.h>
#include "master_asset_manager.h"

namespace OE
{
	class MaterialAssetManager : public AssetManager<Asset::Material>
	{
	public:
		virtual int LoadAsset(const std::string& name, AssetType asset) override;
		virtual int LoadAsset(const std::string& name) override;
		virtual int UnloadAsset(const std::string& name) override;
		virtual void Deserialize(std::string asset_name, YAML::Node node) override;
	};
}
#pragma once
