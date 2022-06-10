#include "vulkan_material_manager.h"

#include "vulkan_material.h"

#include <iostream>

namespace Renderer {

	VulkanMaterialManager::VulkanMaterialManager(Vulkan_type* vulkan_type) : vulkan_type(vulkan_type)
	{
	}

	VulkanMaterialManager::~VulkanMaterialManager()
	{
	}

	int VulkanMaterialManager::AddMaterial(std::string material_name, Material* material)
	{
		if (material_map.find(material_name) != material_map.end())
		{
			std::cout << material_name << " already exists\n";
			return -1;
		}

		material_map[material_name] = std::make_unique<VulkanMaterial>(vulkan_type, material);

		return 0;
	}

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
