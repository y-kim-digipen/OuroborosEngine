#ifndef VULKAN_MATERIAL_H
#define VULKAN_MATERIAL_H

#include "vulkan_type.inl"

#include "../material.h"

#include <memory>

#include "vulkan_texture.h"

namespace Renderer {

	class VulkanUniformBuffer;

	class VulkanMaterial : public Material {

	public:
		//VulkanMaterial(Vulkan_type* vulkan_type, Material* material);
		VulkanMaterial(Vulkan_type* vulkan_type);
		~VulkanMaterial() override;

		void Bind() override;

		bool is_changed = false;
		bool has_texture = false;
		VkDescriptorSetLayout set_layout{};
		std::unique_ptr<VulkanUniformBuffer> ubo;
		std::shared_ptr<VulkanTexture> vulkan_texture;
	private:
		Vulkan_type* vulkan_type;
	
		
		//TODO: does this should go in parent class ???
	};
}

#endif // !VULKAN_MATERIAL_H
