#include "vulkan_material_manager.h"

#include "vulkan_material.h"

#include <iostream>

#include "vulkan_texture_manager.h"

namespace Renderer {

	VulkanMaterialManager::VulkanMaterialManager(VulkanType* vulkan_type) : vulkan_type(vulkan_type)
	{

	}

	VulkanMaterialManager::~VulkanMaterialManager()
	{
		//TODO(Austyn): might need this? 
		//m_non_texture->Cleanup();
	}


	int VulkanMaterialManager::AddMaterial(std::string material_name,const Asset::MaterialData& material_data)
	{
		if (material_map.contains(material_name))
		{
			std::cout << material_name << " already exists\n";
			return -1;
		}

		material_map[material_name] = std::make_unique<VulkanMaterial>(vulkan_type,m_non_texture);
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
			material_map[material_name] = std::make_unique<VulkanMaterial>(vulkan_type, m_non_texture);
			material_map[material_name]->InitMaterialData(data);
		}
		dynamic_cast<VulkanMaterial*>(material_map[material_name].get())->is_changed = true;

		return 0;
	}

;
	

	Material* VulkanMaterialManager::GetMaterial(const std::string& material_name)
	{
		if (!material_map.contains(material_name))
		{
			std::cout << material_name << " doesn't exists\n";
			return nullptr;
		}

		return material_map[material_name].get();
	}

	void VulkanMaterialManager::SetNoneTexture(std::shared_ptr<VulkanTexture> texture)
	{
		m_non_texture = texture;
	}
}
