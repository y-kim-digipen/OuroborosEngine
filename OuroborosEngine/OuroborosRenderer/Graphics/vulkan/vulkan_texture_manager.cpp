#include "vulkan_texture_manager.h"

#include <iostream>

#include "vulkan_texture.h"

namespace Renderer
{
	VulkanTextureManager::VulkanTextureManager(Vulkan_type* vulkan_type) :vulkan_type(vulkan_type)
	{

	}

	int VulkanTextureManager::AddTexture(const std::string& name, const Asset::Image& image)
	{
		if(auto iter = textures_map.find(name); iter == textures_map.end())
		{
			std::shared_ptr<VulkanTexture> new_texture = std::make_shared<VulkanTexture>(vulkan_type);
			new_texture->UploadData(image);
			textures_map.insert({ name,new_texture });
			return 0;
		}
		else
		{
			std::cout << "Texture is already exist!" << std::endl;
			return -1;
		}

	}

	int VulkanTextureManager::DeleteTexture(const std::string& name)
	{
		return 0;


	}
}
