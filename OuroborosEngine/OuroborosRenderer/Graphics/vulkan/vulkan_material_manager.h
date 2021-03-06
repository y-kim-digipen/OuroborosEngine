#ifndef VULKAN_MATERIAL_MANAGER_H
#define VULKAN_MATERIAL_MANAGER_H

#include "vulkan_type.inl"

#include "../material_manager.h"

namespace Renderer {

	class Material;

	class VulkanMaterialManager : public MaterialMananger {
	public:
		VulkanMaterialManager(Vulkan_type* vulkan_type);
		~VulkanMaterialManager() override;
		int AddMaterial(std::string material_name, const Asset::MaterialData& material_data) override;
		int ChangeMaterial(std::string material_name, const Asset::MaterialData& data) override;
		Material* GetMaterial(const std::string& material_name) override;
		friend class VulkanTextureManager;
	private:
		Vulkan_type* vulkan_type;
	};

}

#endif // !VULKAN_MATERIAL_MANAGER_H
