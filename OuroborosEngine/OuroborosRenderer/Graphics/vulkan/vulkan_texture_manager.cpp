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
		static bool init = false;
		if(!init)
		{
			VkDescriptorSetLayoutBinding binding{};
			binding.descriptorCount = 1;
			binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
			binding.binding = 0;

			VkDescriptorSetLayout set_layout;

			VkDescriptorSetLayoutCreateInfo set_layout_create_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
			set_layout_create_info.bindingCount = 1;
			set_layout_create_info.pBindings = &binding;
			VK_CHECK(vkCreateDescriptorSetLayout(vulkan_type->device.handle, &set_layout_create_info, 0, &set_layout));

			VkDescriptorSetAllocateInfo alloc_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
			alloc_info.descriptorPool = vulkan_type->descriptor_pool;
			alloc_info.descriptorSetCount = 1;
			alloc_info.pSetLayouts = &set_layout;

			VK_CHECK(vkAllocateDescriptorSets(vulkan_type->device.handle, &alloc_info, &descriptor_set_));
			init = true;
		}

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
