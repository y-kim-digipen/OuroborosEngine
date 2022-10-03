#include "vulkan_material.h"

#include <array>

#include "vulkan_buffer.h"

namespace Renderer {

	VulkanMaterial::VulkanMaterial(VulkanType* vulkan_type, std::shared_ptr<Texture> none_texture) : vulkan_type(vulkan_type), ubo(std::make_unique<VulkanUniformBuffer>(vulkan_type, 0, sizeof(Asset::MaterialData)))
	{
		set.Init(vulkan_type, 2)
			.AddBindingLayout(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.AddBindingLayout(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.AddBindingLayout(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.AddBindingLayout(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.AddBindingLayout(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.AddBindingLayout(5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.AddBindingLayout(6, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);

		SetAlbedoTexture(none_texture);
		SetMetalRoughnessTexture(none_texture);
		SetNormalTexture(none_texture);
		SetMetalicTexture(none_texture);
		SetRoughSmoothnessTexture(none_texture);
		SetEmissiveTexture(none_texture);
		SetAOTexture(none_texture);

		ubo->AddData(&data, 0, sizeof(data));

		//ubo->AddBinding(0, sizeof(Asset::MaterialData));
		//ubo->SetupDescriptorSet(set_layout);
		//ubo->AddData(0 ,&data, 0, sizeof(data));
		//ubo->UploadToGPU(0);
	}

	VulkanMaterial::~VulkanMaterial()
	{
		Cleanup();
	}

	void VulkanMaterial::SetAlbedoTexture(std::shared_ptr<Texture> texture)
	{
		texture_albedo = texture;
		data.has_albedo_texture = true;
	}

	void VulkanMaterial::Bind()
	{
		if (is_changed)
		{
			//ubo->AddData(0, &data, 0, sizeof(Asset::MaterialData));
			//ubo->UploadToGPU(0);
			//is_changed = false;
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
		if (data.has_metalic_texture)
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
	void VulkanMaterial::Cleanup()
	{
		set.Cleanup();

		ubo.reset();
	}
}
