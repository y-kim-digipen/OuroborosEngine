#include "vulkan_material_manager.h"

#include "vulkan_material.h"

#include <iostream>

namespace Renderer {

	VulkanMaterialManager::VulkanMaterialManager(VulkanType* vulkan_type) : vulkan_type(vulkan_type)
	{

	}

	VulkanMaterialManager::~VulkanMaterialManager()
	{
	}


	int VulkanMaterialManager::AddMaterial(std::string material_name,const Asset::MaterialData& material_data)
	{
		if (material_map.find(material_name) != material_map.end())
		{
			std::cout << material_name << " already exists\n";
			return -1;
		}
		material_map[material_name] = std::make_unique<VulkanMaterial>(vulkan_type);
		material_map[material_name]->InitMaterialData(std::move(material_data));
		dynamic_cast<VulkanMaterial*>(material_map[material_name].get())->is_changed = true;

		return 0;
	}

	int VulkanMaterialManager::ChangeMaterial(std::string material_name,const Asset::MaterialData& data)
	{
		if (auto iter = material_map.find(material_name); iter != material_map.end())
		{
			iter->second->InitMaterialData(data);
		}
		else
		{
			material_map[material_name] = std::make_unique<VulkanMaterial>(vulkan_type);
			material_map[material_name]->InitMaterialData(data);
		}
		dynamic_cast<VulkanMaterial*>(material_map[material_name].get())->is_changed = true;

		return 0;
	}

;
	

	Material* VulkanMaterialManager::GetMaterial(const std::string& material_name)
	{
		if (material_map.find(material_name) == material_map.end())
		{
			std::cout << material_name << " doesn't exists\n";
			return nullptr;
		}

		return material_map[material_name].get();
	}
}
