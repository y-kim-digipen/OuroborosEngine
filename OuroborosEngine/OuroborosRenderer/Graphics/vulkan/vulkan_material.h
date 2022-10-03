#ifndef VULKAN_MATERIAL_H
#define VULKAN_MATERIAL_H

#include "vulkan_type.inl"

#include "../material.h"

#include <memory>

#include "vulkan_texture.h"
#include "vulkan_descriptor_set.h"

namespace Renderer {

	class VulkanUniformBuffer;

	class VulkanMaterial : public Material {

	public:
		//VulkanMaterial(Vulkan_type* vulkan_type, Material* material);
		VulkanMaterial(VulkanType* vulkan_type ,std::shared_ptr<VulkanTexture> noneTexutre);
		~VulkanMaterial() override;
		void SetAlbedoTexture(std::shared_ptr<Texture> texture) override;
		void Bind() override;
		void SetAOTexture(std::shared_ptr<Texture> texture) override;
		void SetMetalRoughnessTexture(std::shared_ptr<Texture> texture) override;
		void SetNormalTexture(std::shared_ptr<Texture> texture) override;
		void SetMetalicTexture(std::shared_ptr<Texture> texture) override;
		void SetRoughSmoothnessTexture(std::shared_ptr<Texture> texture) override;
		void SetEmissiveTexture(std::shared_ptr<Texture> texture) override;
		void Cleanup() override;

		bool is_changed = false;

		std::unique_ptr<VulkanUniformBuffer> ubo;
		DescriptorSet set;

	private:
		VulkanType* vulkan_type;
	};
}

#endif // !VULKAN_MATERIAL_H
