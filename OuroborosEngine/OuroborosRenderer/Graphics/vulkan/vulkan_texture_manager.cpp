#include "vulkan_texture_manager.h"

#include <iostream>

#include "../texture.h"

namespace Renderer
{
	VulkanTextureManager::VulkanTextureManager(VulkanType* vulkan_type) :vulkan_type(vulkan_type), vulkan_texture_imgui_descriptor_pool(vulkan_type)
	{
		
	}

	int VulkanTextureManager::AddTexture(const std::string& name, const Asset::Image& image)
	{
		static bool init = false;
		if(!init)
		{
			vulkan_texture_imgui_descriptor_pool.Init();
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
		if(auto iter = textures_map.find(name); iter != textures_map.end())
		{
			textures_map.erase(iter);
			return 0;
		}
		else
		{
			std::cout << name << " doesn't exist! cannot delete it" << std::endl;
			return -1;
		}


	}

	VulkanTextureImguiDescriptorPool::VulkanTextureImguiDescriptorPool(VulkanType* vulkan_type) : vulkan_type(vulkan_type), descriptor_sets_pool(10, VkDescriptorSet())
	{

	}

	VulkanTextureImguiDescriptorPool::~VulkanTextureImguiDescriptorPool()
	{
		for(auto descriptorset : descriptor_sets_pool)
		{
			vkFreeDescriptorSets(vulkan_type->device.handle, vulkan_type->descriptor_pool, 1, &descriptorset);
		}
	}

	VkDescriptorSet* VulkanTextureImguiDescriptorPool::GetImGuiTextureID()
	{
		if(NeedGrowCapacity())
		{
			GrowCapacity((current_container_size + 10) * 2);
		}
	
		return &descriptor_sets_pool[current_used_id_num++];
	}

	void VulkanTextureImguiDescriptorPool::Init()
	{
		std::vector<VkDescriptorSetLayoutBinding> bindings;
		//max imgui texture 4

		VkDescriptorSetLayoutBinding binding{};
		binding.descriptorCount = 1;
		binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		binding.binding = 0;
		bindings.push_back(binding);


		for (int i = 0; i < current_container_size; ++i)
		{
			VkDescriptorSetLayout set_layout;

			VkDescriptorSetLayoutCreateInfo set_layout_create_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
			set_layout_create_info.bindingCount = 1;
			set_layout_create_info.pBindings	= bindings.data();
			VK_CHECK(vkCreateDescriptorSetLayout(vulkan_type->device.handle, &set_layout_create_info, 0, &set_layout));

			VkDescriptorSetAllocateInfo alloc_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
			alloc_info.descriptorPool	  = vulkan_type->descriptor_pool;
			alloc_info.descriptorSetCount = 1;
			alloc_info.pSetLayouts		  = &set_layout;

			VK_CHECK(vkAllocateDescriptorSets(vulkan_type->device.handle, &alloc_info, &descriptor_sets_pool[i]));
		}
	}

	bool VulkanTextureImguiDescriptorPool::NeedGrowCapacity() const
	{
		return current_used_id_num + 5 >= current_container_size;
	}

	void VulkanTextureImguiDescriptorPool::GrowCapacity(int size)
	{
		std::vector<VkDescriptorSetLayoutBinding> bindings;
		//max imgui texture 4

		VkDescriptorSetLayoutBinding binding{};
		binding.descriptorCount = 1;
		binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		binding.binding = 0;
		bindings.push_back(binding);

		for(int i = 0; i < size - current_used_id_num; i++)
		{
			VkDescriptorSet descriptor_set;
			VkDescriptorSetLayout set_layout;

			VkDescriptorSetLayoutCreateInfo set_layout_create_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
			set_layout_create_info.bindingCount = 1;
			set_layout_create_info.pBindings	= bindings.data();
			VK_CHECK(vkCreateDescriptorSetLayout(vulkan_type->device.handle, &set_layout_create_info, 0, &set_layout));

			VkDescriptorSetAllocateInfo alloc_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
			alloc_info.descriptorPool	  = vulkan_type->descriptor_pool;
			alloc_info.descriptorSetCount = 1;
			alloc_info.pSetLayouts		  = &set_layout;

			VK_CHECK(vkAllocateDescriptorSets(vulkan_type->device.handle, &alloc_info, &descriptor_set));
			descriptor_sets_pool.push_back(descriptor_set);
		}

		current_container_size = size;
	}

	void VulkanTextureImguiDescriptorPool::ResetCount()
	{
		current_used_id_num = 0;
	}
}
