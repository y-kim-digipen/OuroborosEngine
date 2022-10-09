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

		void SetTexture(Shared::PBR_TEXTURE_TYPES type, std::shared_ptr<Texture> texture) override;
		void Cleanup() override;

		bool is_changed = false;
		VkDescriptorSetLayout set_layout{};
		std::unique_ptr<VulkanUniformBuffer> ubo;
	
	private:
		Vulkan_type* vulkan_type;
	
		
		//TODO: does this should go in parent class ???
	};
}

#endif // !VULKAN_MATERIAL_H
