#include "scene_serializer.h"
#include <../src/engine/engine.h>
#include <fstream>

#include "../input/InputManager.h"
#include "../src/engine/engine_settings.h"
#include "../src/engine/engine.h"

namespace OE
{
	void SceneSerializer::Init()
	{
		emitter = new YAML::Emitter;
	}

	void SceneSerializer::SerializeScene(const std::string& path)
	{
		assert(emitter);
		YAML::Emitter& out = *emitter;
		auto& ecs_manager = OE::Engine::GetECSManager();
		auto& master_asset_manager = OE::Engine::asset_manager;
		out << YAML::BeginMap;

		//Asset info
		out << YAML::Key << "Asset";
		out << YAML::Value;
		out << YAML::BeginMap;
		brigand::for_each<Asset_Manager>([&](auto t)
			{
				using TAssetManager = typename decltype(t)::type;
				out << YAML::Key << typeid(TAssetManager).name();
				out << YAML::Value;
				master_asset_manager.GetManager<TAssetManager>().Serialize(out);
			});
		out << YAML::EndMap;
		//ECS info
		out << YAML::Key << "ECS";
		out << YAML::Value;
		out << YAML::BeginMap;
		int entity_counter = 0;
		ecs_manager.ForEntities([&](OE::ecs_ID entityID)
			{
				//EntityID
				out << YAML::Key << entity_counter++;
				out << YAML::Value << YAML::BeginMap;
				//System
				out << YAML::Key << "System";
				out << YAML::Value << YAML::BeginMap;
				brigand::for_each<SystemList>([&]<typename T>(T /*T*/)
				{
					using TSystem = typename T::type;
					out << YAML::Key << typeid(TSystem).name();
					const bool has_system = ecs_manager.HasSystem<TSystem>(entityID);
					out << YAML::Value << YAML::YesNoBool << has_system;
				}
				);
				out << YAML::EndMap;

				out << YAML::Key << "Component";
				out << YAML::Value << YAML::BeginMap;
				brigand::for_each<ComponentList>([&]<typename T>(T /*T*/)
				{
					using TComponent = typename T::type;
					const bool has_component = ecs_manager.HasComponent<TComponent>(entityID);
					if (has_component)
					{
						out << YAML::Key << typeid(TComponent).name();
						out << YAML::Value << ecs_manager.GetComponent<TComponent>(entityID);
					}
				}
				);
				out << YAML::EndMap;
				out << YAML::EndMap;
			});
		out << YAML::EndMap;

		out << YAML::EndMap;
		EmitToFile(path);
	}

	void SceneSerializer::DeserializeScene(const std::string& file_name)
	{
		auto& ecs_manager = OE::Engine::GetECSManager();
		auto& master_asset_manager = OE::Engine::asset_manager;
		ecs_manager.Clear();
		YAML::Node scene_file = YAML::LoadFile(file_name);

		
		auto asset_node = scene_file["Asset"];
		master_asset_manager.CleanUp();
		
		for (YAML::iterator it = asset_node.begin(); it != asset_node.end(); ++it)
		{
			const std::string& asset_manager_name = it->first.as<std::string>();

			YAML::Node asset_sequence_node = it->second;

			bool stop_search = false;
			brigand::for_each<Asset_Manager>([asset_manager_name, stop_search, &master_asset_manager, asset_sequence_node](auto t) mutable
				{
					if(stop_search == false)
					{
						using TAssetManager = typename decltype(t)::type;
						if (asset_manager_name == typeid(TAssetManager).name())
						{
							auto& asset_manager = master_asset_manager.GetManager<TAssetManager>();

							for(YAML::iterator it = asset_sequence_node.begin(); it != asset_sequence_node.end(); ++it)
							{
								const std::string& asset_name = it->as<std::string>();
								asset_manager.LoadAsset(asset_name);
							}
							stop_search = true;
						}
					}
				});
		}



		auto ecs_node = scene_file["ECS"];
		//YAML::LOAD
		int entity_counter = 0;

		for (YAML::iterator it = ecs_node.begin(); it != ecs_node.end(); ++it)
		{
			int original_entID = it->first.as<int>();
			auto ent = ecs_manager.CreateEntity();
			OE::ecs_ID entity_ID = ent.myID;
			YAML::Node ent_info_map = it->second;
			YAML::Node system_map = ent_info_map["System"];
			YAML::Node component_map = ent_info_map["Component"];

			for (YAML::iterator system_it = system_map.begin(); system_it != system_map.end(); ++system_it)
			{
				std::string system_name = system_it->first.as<std::string>();
				bool	    system_usage = system_it->second.as<bool>();
				if (system_usage == false)
				{
					continue;
				}
				bool		stop_foreach = false;
				brigand::for_each<SystemList>([system_name, &stop_foreach, &ecs_manager, entity_ID](auto t) mutable
					{
						if (stop_foreach == false)
						{
							using TSystem = typename decltype(t)::type;
							if (typeid(TSystem).name() == system_name)
							{
								ecs_manager.AddSystem<TSystem>(entity_ID);
								stop_foreach = true;
							}
						}
					});
			}

			for (YAML::iterator component_it = component_map.begin(); component_it != component_map.end(); ++component_it)
			{
				std::string component_name = component_it->first.as<std::string>();
				bool		stop_foreach = false;
				brigand::for_each<ComponentList>([component_name, &stop_foreach, &ecs_manager, entity_ID, component_it](auto t) mutable
					{
						if (stop_foreach == false)
						{
							using TComponent = typename decltype(t)::type;
							if (typeid(TComponent).name() == component_name)
							{
								TComponent component;
								YAML::Node data = component_it->second;
								data >> component;
								ecs_manager.AddComponent<TComponent>(entity_ID, component);
								stop_foreach = true;
							}
						}
					});
				}
			}
		}

	void SceneSerializer::CleanUp()
	{
		if(emitter)
		{
			delete emitter;
		}
	}

	//	for (const YAML::Node& entity_info_node : scene_file)
	//	{
	//		const std::string key = entity_info_node.first;
	//		assert(entity_info_node.IsMap());

	//		//auto system_info_node = entity_info_node["System"];
	///*		for (const auto& system : system_info_node)
	//		{
	//			std::cout << system;
	//		}*/
	//		entity_counter++;

	//		//ecs_manager.AddSystem<>()
	//		//std::cout << entity_info_node << std::endl;
	//	}



	void SceneSerializer::EmitToFile(const std::string& file_name)
	{
		std::ofstream ostrm;

		ostrm.open(file_name, std::ios::binary | std::ios::trunc);

		assert(ostrm.is_open());
		YAML::Emitter& out = *emitter;
		ostrm << out.c_str();
		delete emitter;
		emitter = new YAML::Emitter;
	}
}
