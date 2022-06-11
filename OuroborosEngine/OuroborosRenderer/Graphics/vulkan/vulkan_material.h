#ifndef VULKAN_MATERIAL_H
#define VULKAN_MATERIAL_H

#include "vulkan_type.inl"

#include "../material.h"

#include <memory>

namespace Renderer {

	class VulkanUniformBuffer;

	class VulkanMaterial : public Material {

	public:
		VulkanMaterial(Vulkan_type* vulkan_type, Material* material);
		VulkanMaterial(Vulkan_type* vulkan_type);
		~VulkanMaterial() override;

		void Bind() override;

		bool is_changed = false;
	private:
		Vulkan_type* vulkan_type;
		VkDescriptorSetLayout set_layout{};
		//TODO: does this should go in parent class ???
		std::unique_ptr<VulkanUniformBuffer> ubo;
	};
}

#endif // !VULKAN_MATERIAL_H
