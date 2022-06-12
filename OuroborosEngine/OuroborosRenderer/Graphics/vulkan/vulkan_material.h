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
		VkDescriptorSetLayout set_layout{};
		std::unique_ptr<VulkanUniformBuffer> ubo;
	private:
		Vulkan_type* vulkan_type;
		//TODO: does this should go in parent class ???
	};
}

#endif // !VULKAN_MATERIAL_H
