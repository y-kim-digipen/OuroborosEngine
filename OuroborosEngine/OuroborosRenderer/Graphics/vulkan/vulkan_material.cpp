#include "vulkan_material.h"

#include <array>

#include "vulkan_buffer.h"

namespace Renderer {

	VulkanMaterial::VulkanMaterial(VulkanType* vulkan_type, std::shared_ptr<VulkanTexture> none_texture) : vulkan_type(vulkan_type), ubo(std::make_unique<VulkanUniformBuffer>(vulkan_type, 0, sizeof(Asset::MaterialData)))
	{
		set.Init(vulkan_type, 2)
			.AddBindingLayout(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.AddBindingLayout(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.AddBindingLayout(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.AddBindingLayout(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.AddBindingLayout(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.AddBindingLayout(5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.AddBindingLayout(6, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);

			textures[Shared::ALBEDO] = none_texture;
			textures[Shared::NORMAL] = none_texture;
			textures[Shared::METALLIC_ROUGHNESS] = none_texture;
			textures[Shared::METALLIC] = none_texture;
			textures[Shared::ROUGHNESS] = none_texture;
			textures[Shared::AO] = none_texture;
			textures[Shared::EMISSIVE] = none_texture;

		set.AddBinding(0, ubo.get())
			.AddBinding(1, none_texture.get())
			.AddBinding(2, none_texture.get())
			.AddBinding(3, none_texture.get())
			.AddBinding(4, none_texture.get())
			.AddBinding(5, none_texture.get())
			.AddBinding(6, none_texture.get())
			.Build();



		ubo->AddData(&data, 0, sizeof(data));
	}

	VulkanMaterial::~VulkanMaterial()
	{
		Cleanup();
	}

	void VulkanMaterial::Bind()
	{
		if (is_changed)
		{
			ubo->AddData(&data, 0, sizeof(Asset::MaterialData));
			is_changed = false;
		}
		if (data.has_albedo_texture)
		{
			if (auto* const ptr = textures[Shared::ALBEDO].get(); ptr != nullptr)
			{
				set.AddBinding(1, ptr);
			}
		}
		if (data.has_normal_texture)
		{
			if (auto* const ptr = textures[Shared::NORMAL].get(); ptr != nullptr)
			{
				set.AddBinding(2, ptr);
			}
		}
		if (data.has_metalroughness_texture)
		{
			if (auto* const ptr = textures[Shared::METALLIC_ROUGHNESS].get(); ptr != nullptr)
			{
				set.AddBinding(3, ptr);
			}
		}
		if (data.has_ao_texture)
		{
			if (auto* const ptr = textures[Shared::AO].get(); ptr != nullptr)
			{
				set.AddBinding(4, ptr);
			}
		}
		if (data.has_metalic_texture)
		{
			if (auto* const ptr = textures[Shared::METALLIC].get(); ptr != nullptr)
			{
				set.AddBinding(5, ptr);
			}
		}
		if (data.has_roughness_texture)
		{
			if (auto* const ptr = textures[Shared::ROUGHNESS].get(); ptr != nullptr)
			{
				set.AddBinding(6, ptr);
			}
		}
		if (data.has_emissive_texture)
		{
			if (auto* const ptr = textures[Shared::EMISSIVE].get(); ptr != nullptr)
			{

				set.AddBinding(7, ptr);
			}
		}

		set.Bind();
	}

	void VulkanMaterial::SetTexture(Shared::PBR_TEXTURE_TYPES type, std::shared_ptr<VulkanTexture> texture)
	{
		Material::SetTexture(type, texture);
	}


	void VulkanMaterial::Cleanup()
	{
		set.Cleanup();
		ubo.reset();
	}
}
