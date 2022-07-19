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

		VkDescriptorSetLayoutBinding sampler_layout_binding_1{};
		sampler_layout_binding_1.binding				= 1;
		sampler_layout_binding_1.descriptorCount		= 1;
		sampler_layout_binding_1.descriptorType		= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		sampler_layout_binding_1.pImmutableSamplers	= nullptr;
		sampler_layout_binding_1.stageFlags			= VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutBinding sampler_layout_binding_2{};
		sampler_layout_binding_2.binding = 2;
		sampler_layout_binding_2.descriptorCount = 1;
		sampler_layout_binding_2.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		sampler_layout_binding_2.pImmutableSamplers = nullptr;
		sampler_layout_binding_2.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutBinding sampler_layout_binding_3{};
		sampler_layout_binding_3.binding = 3;
		sampler_layout_binding_3.descriptorCount = 1;
		sampler_layout_binding_3.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		sampler_layout_binding_3.pImmutableSamplers = nullptr;
		sampler_layout_binding_3.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutBinding sampler_layout_binding_4{};
		sampler_layout_binding_4.binding = 4;
		sampler_layout_binding_4.descriptorCount = 1;
		sampler_layout_binding_4.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		sampler_layout_binding_4.pImmutableSamplers = nullptr;
		sampler_layout_binding_4.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		std::array<VkDescriptorSetLayoutBinding, 5> bindings = { binding, sampler_layout_binding_1, sampler_layout_binding_2, sampler_layout_binding_3, sampler_layout_binding_4 };

		VkDescriptorSetLayoutCreateInfo set_layout_create_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
		set_layout_create_info.bindingCount = 5;
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

	void VulkanMaterial::SetAlbedoTexture(std::shared_ptr<Texture> texture)
	{
		texture_albedo = texture;
		data.has_albedo_texture = true;
	}

	void VulkanMaterial::Bind()
	{
		if(is_changed)
		{
			ubo->AddData(&data, 0, sizeof(Asset::MaterialData));
			ubo->UploadToGPU();
			is_changed = false;
		}

				if (data.has_albedo_texture)
				{
					if (auto* const ptr = dynamic_cast<VulkanTexture*>(texture_albedo.get()); ptr != nullptr)
					{
						ptr->UpdateToDescripterSet(ubo->descriptor_set[vulkan_type->current_frame], 1);
					}
				}
				if (data.has_normal_texture)
				{
					if (auto* const ptr = dynamic_cast<VulkanTexture*>(texture_normal.get()); ptr != nullptr)
					{
						ptr->UpdateToDescripterSet(ubo->descriptor_set[vulkan_type->current_frame], 2);
					}
				}
				if (data.has_metalroughness_texture)
				{
					if (auto* const ptr = dynamic_cast<VulkanTexture*>(texture_metalroughness.get()); ptr != nullptr)
					{
						ptr->UpdateToDescripterSet(ubo->descriptor_set[vulkan_type->current_frame], 3);
					}
				}
				if (data.has_ao_texture)
				{
					if (auto* const ptr = dynamic_cast<VulkanTexture*>(texture_ao.get()); ptr != nullptr)
					{
						ptr->UpdateToDescripterSet(ubo->descriptor_set[vulkan_type->current_frame], 4);
					}
				}
			
		ubo->Bind();
	}

	void VulkanMaterial::SetAOTexture(std::shared_ptr<Texture> texture)
	{
		Material::SetAOTexture(texture);
		data.has_ao_texture = true;
	}

	void VulkanMaterial::SetMetalRoughness(std::shared_ptr<Texture> texture)
	{
		Material::SetMetalRoughness(texture);
		data.has_metalroughness_texture = true;
	}

	void VulkanMaterial::SetNormalTexture(std::shared_ptr<Texture> texture)
	{
		Material::SetNormalTexture(texture);
		data.has_normal_texture = true;
	}
}
