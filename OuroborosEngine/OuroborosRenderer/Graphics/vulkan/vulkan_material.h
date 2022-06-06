#ifndef VULKAN_MATERIAL_H
#define VULKAN_MATERIAL_H

#include "vulkan_type.inl"

#include "../material.h"

#include <memory>

namespace Renderer {

	class VulkanUniformBuffer;

	class VulkanMaterial : public Material {

	public:
		VulkanMaterial(Vulkan_type* vulkan_type);
		~VulkanMaterial() override;

		void Bind() override;
	private:
		Vulkan_type* vulkan_type;
		
		//TODO: does this should go in parent class ???
		std::unique_ptr<VulkanUniformBuffer> ubo;
	};
}

#endif // !VULKAN_MATERIAL_H
