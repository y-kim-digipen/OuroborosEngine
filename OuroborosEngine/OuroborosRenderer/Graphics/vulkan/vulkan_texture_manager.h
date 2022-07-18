#ifndef VULKAN_TEXTURE_MANAGER_H
#define VULKAN_TEXTURE_MANAGER_H
#include <vulkan_core.h>

#include "../texture_manager.h"

struct Vulkan_type;

namespace Renderer
{
	class VulkanTextureManager : public TextureManager
	{
	public:
		VulkanTextureManager(Vulkan_type* vulkan_type);
		int AddTexture(const std::string& name, const Asset::Image& image) override;
		int DeleteTexture(const std::string& name) override;
		VkDescriptorSet descriptor_set_[4];
	private:
		Vulkan_type* vulkan_type;
	};
}
#endif //!VULKAN_TEXTURE_MANAGER_H