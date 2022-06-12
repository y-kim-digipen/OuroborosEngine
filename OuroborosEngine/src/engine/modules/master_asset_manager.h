#pragma once
#include <ranges>
#include <map>

namespace OE
{
	template<typename TAsset>
	class AssetManager
	{
	public:
		virtual ~AssetManager() = default;
		virtual int LoadAsset(const std::string& name) = 0;
		virtual int UnloadAsset(const std::string& name)
		{
			if(!assets.contains(name))
			{
				return 1;
			}
			assets.erase(assets.find(name));
			return 0;
		}
        TAsset& GetAsset(std::string&& asset_name)
        {
            return assets[asset_name].second;
        }
        bool HasAsset(std::string&& asset_name)
        {
            return assets.contains(asset_name);
        }
        bool IsAssetReady(std::string&& asset_name)
        {
	        if(!HasAsset(asset_name))
	        {
                return false;
	        }
            return assets[asset_name].first;
        }

		[[nodiscard]] const std::map<std::string, std::pair<bool, TAsset>>& GetAssetRawData() const
        {
			return assets;
        }
	protected:
		std::map<std::string, std::pair<bool, TAsset>> assets;
	};

	template<typename ...TManagers>
	class MasterAssetManager
	{
	public:
		template<typename TManager>
		TManager& GetManager()
		{
			return std::get<TManager>(tuple_vector_of_assets);
		}

		std::tuple<TManagers...> tuple_vector_of_assets;
	};
}

