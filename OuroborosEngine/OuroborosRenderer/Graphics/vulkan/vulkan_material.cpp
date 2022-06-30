#include "vulkan_material.h"

#include <array>

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
		VkDescriptorSetLayoutBinding binding{};
		binding.binding = 0;
		binding.descriptorCount = 1;
		binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutBinding sampler_layout_binding{};
		sampler_layout_binding.binding				= 1;
		sampler_layout_binding.descriptorCount		= 1;
		sampler_layout_binding.descriptorType		= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		sampler_layout_binding.pImmutableSamplers	= nullptr;
		sampler_layout_binding.stageFlags			= VK_SHADER_STAGE_FRAGMENT_BIT;


		std::array<VkDescriptorSetLayoutBinding, 2> bindings = { binding, sampler_layout_binding };

		VkDescriptorSetLayoutCreateInfo set_layout_create_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
		set_layout_create_info.bindingCount = 2;
		set_layout_create_info.pBindings    = bindings.data();


		VK_CHECK(vkCreateDescriptorSetLayout(vulkan_type->device.handle, &set_layout_create_info, 0, &set_layout));

		ubo->AddBinding(0, sizeof(Asset::MaterialData));
		ubo->SetupDescriptorSet(set_layout);
		ubo->AddData(&data, 0, sizeof(data));
		ubo->UploadToGPU();


	}

	VulkanMaterial::~VulkanMaterial()
	{
		vkDestroyDescriptorSetLayout(vulkan_type->device.handle, set_layout, nullptr);
	}

	void VulkanMaterial::SetTexture(std::shared_ptr<Texture> texture)
	{
		texture_ = texture;
		has_texture = true;
	}

	void VulkanMaterial::Bind()
	{
		if(is_changed)
		{
			ubo->AddData(&data, 0, sizeof(Asset::MaterialData));
			ubo->UploadToGPU();
			is_changed = false;
		}
		if (has_texture)
		{
			for (int i = 0; i < 3; ++i)
				dynamic_cast<VulkanTexture*>(texture_.get())->UpdateToDescripterSet(ubo->descriptor_set[i], 1);
		}

		ubo->Bind();
	}
}