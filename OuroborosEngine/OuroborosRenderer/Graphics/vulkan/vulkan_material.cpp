#include "vulkan_material.h"

#include "vulkan_buffer.h"

namespace Renderer {

	/*
	//TODO: Material(*material) ???
	VulkanMaterial::VulkanMaterial(Vulkan_type* vulkan_type, Material* material) : Material(*material), vulkan_type(vulkan_type), ubo(std::make_unique<VulkanUniformBuffer>(vulkan_type, 2))
	{
		//TODO: might not need to create descriptor set layout everytime
		VkDescriptorSetLayoutBinding binding;
		binding.binding = 0;
		binding.descriptorCount = 1;
		binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		
		VkDescriptorSetLayoutCreateInfo set_layout_create_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
		set_layout_create_info.bindingCount = 1;
		set_layout_create_info.pBindings = &binding;

		VkDescriptorSetLayout set_layout{};

		VK_CHECK(vkCreateDescriptorSetLayout(vulkan_type->device.handle, &set_layout_create_info, 0, &set_layout));
		ubo->AddBinding(0, sizeof(Material));
		ubo->AddData(&data, 0, sizeof(data));
		ubo->SetupDescriptorSet(1, set_layout);
	}
	*/

	VulkanMaterial::VulkanMaterial(Vulkan_type* vulkan_type) : vulkan_type(vulkan_type) , ubo(std::make_unique<VulkanUniformBuffer>(vulkan_type, 2))
	{
		VkDescriptorSetLayoutBinding binding;
		binding.binding = 0;
		binding.descriptorCount = 1;
		binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutCreateInfo set_layout_create_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
		set_layout_create_info.bindingCount = 1;
		set_layout_create_info.pBindings    = &binding;


		VK_CHECK(vkCreateDescriptorSetLayout(vulkan_type->device.handle, &set_layout_create_info, 0, &set_layout));

		ubo->AddBinding(0, sizeof(Asset::MaterialData));
		ubo->SetupDescriptorSet(1, set_layout);
		ubo->AddData(&data, 0, sizeof(data));
		ubo->UploadToGPU();


		if(has_texture)
		{
			for (int i = 0; i < 3; ++i)
				vulkan_texture->UpdateToDescripterSet(ubo->descriptor_set[i], 1);
		}

	}

	VulkanMaterial::~VulkanMaterial()
	{
		vkDestroyDescriptorSetLayout(vulkan_type->device.handle, set_layout, nullptr);
	}

	void VulkanMaterial::Bind()
	{
		if(is_changed)
		{
			ubo->AddData(&data, 0, sizeof(Asset::MaterialData));
			ubo->UploadToGPU();
			is_changed = false;
		}

		ubo->Bind();
	}
}