#include "vulkan_material.h"

#include <array>

#include "vulkan_buffer.h"

namespace Renderer {

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

		VkDescriptorSetLayoutBinding sampler_layout_binding_5{};
		sampler_layout_binding_5.binding = 5;
		sampler_layout_binding_5.descriptorCount = 1;
		sampler_layout_binding_5.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		sampler_layout_binding_5.pImmutableSamplers = nullptr;
		sampler_layout_binding_5.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutBinding sampler_layout_binding_6{};
		sampler_layout_binding_6.binding = 6;
		sampler_layout_binding_6.descriptorCount = 1;
		sampler_layout_binding_6.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		sampler_layout_binding_6.pImmutableSamplers = nullptr;
		sampler_layout_binding_6.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		

		std::array<VkDescriptorSetLayoutBinding, 7> bindings = { binding, sampler_layout_binding_1, sampler_layout_binding_2, sampler_layout_binding_3, sampler_layout_binding_4, sampler_layout_binding_5, sampler_layout_binding_6 };

		VkDescriptorSetLayoutCreateInfo set_layout_create_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
		set_layout_create_info.bindingCount = 7;
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
				if(data.has_metalic_texture)
				{
					if (auto* const ptr = dynamic_cast<VulkanTexture*>(texture_ao.get()); ptr != nullptr)
					{
						ptr->UpdateToDescripterSet(ubo->descriptor_set[vulkan_type->current_frame], 3);
					}
				}
				if (data.has_roughness_texture)
				{
					if (auto* const ptr = dynamic_cast<VulkanTexture*>(texture_ao.get()); ptr != nullptr)
					{
						ptr->UpdateToDescripterSet(ubo->descriptor_set[vulkan_type->current_frame], 5);
					}
				}
				if (data.has_emissive_texture)
				{
					if (auto* const ptr = dynamic_cast<VulkanTexture*>(texture_emissive.get()); ptr != nullptr)
					{
						ptr->UpdateToDescripterSet(ubo->descriptor_set[vulkan_type->current_frame], 6);
					}
				}
			
		ubo->Bind();
	}

	void VulkanMaterial::SetAOTexture(std::shared_ptr<Texture> texture)
	{
		Material::SetAOTexture(texture);
		data.has_ao_texture = true;
	}

	void VulkanMaterial::SetMetalRoughnessTexture(std::shared_ptr<Texture> texture)
	{
		Material::SetMetalRoughnessTexture(texture);
		data.has_metalroughness_texture = true;
	}

	void VulkanMaterial::SetNormalTexture(std::shared_ptr<Texture> texture)
	{
		Material::SetNormalTexture(texture);
		data.has_normal_texture = true;
	}

	void VulkanMaterial::SetMetalicTexture(std::shared_ptr<Texture> texture)
	{
		Material::SetMetalicTexture(texture);
		data.has_metalic_texture = true;
	}

	void VulkanMaterial::SetRoughSmoothnessTexture(std::shared_ptr<Texture> texture)
	{
		Material::SetRoughSmoothnessTexture(texture);
		data.has_roughness_texture = true;
	}

	void VulkanMaterial::SetEmissiveTexture(std::shared_ptr<Texture> texture)
	{
		Material::SetEmissiveTexture(texture);
		data.has_emissive_texture = true;
	}
}
