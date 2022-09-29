#pragma once
#include <ranges>
#include <map>
#include <yaml-cpp/yaml.h>
#include <brigand/algorithms.hpp>
#include <iostream>
namespace OE
{
	template<typename TAsset>
	class AssetManager
	{
	public:
		using asset_type = TAsset;
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
		bool HasAsset(const std::string& asset_name)
		{
			return assets.contains(asset_name);
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

		virtual void CleanUp()
		{
			assets.clear();
		}

		void Serialize(YAML::Emitter& emitter) const
		{
			emitter << YAML::BeginSeq;
			for (auto element : assets)
			{
				std::cout << element.first << std::endl;
				emitter << std::string(element.first);
			}
			emitter << YAML::EndSeq;
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

		void CleanUp()
		{
			brigand::for_each<decltype(tuple_vector_of_assets)>([this](auto t)
				{
					GetManager<typename decltype(t)::type>().CleanUp();
				});
		}
		using manager_list = brigand::list<TManagers...>;
		std::tuple<TManagers...> tuple_vector_of_assets;
	};
}

