#ifndef VULKAN_TEXTURE_MANAGER_H
#define VULKAN_TEXTURE_MANAGER_H
#include <vulkan/vulkan_core.h>

#include "../texture_manager.h"

struct VulkanType;

namespace Renderer
{
	using TextureID = VkDescriptorSet;
	

	class VulkanTextureImguiDescriptorPool
	{
	public:
		VulkanTextureImguiDescriptorPool(VulkanType* vulkan_type);
		~VulkanTextureImguiDescriptorPool();
		TextureID* GetImGuiTextureID();
		void Init();
		[[nodiscard]] bool NeedGrowCapacity() const;
		void GrowCapacity(int size);
		void ResetCount();
	private:
		int current_container_size = 10;
		int current_used_id_num = 0;
		std::vector<VkDescriptorSet> descriptor_sets_pool;
		VulkanType* vulkan_type;
	};
	class VulkanTextureManager : public TextureManager
	{
	public:
		VulkanTextureManager(VulkanType* vulkan_type);
		int AddTexture(const std::string& name, const Asset::Image& image) override;
		int DeleteTexture(const std::string& name) override;
		int Cleanup() override;

		VulkanTextureImguiDescriptorPool vulkan_texture_imgui_descriptor_pool;
	private:
		VulkanType* vulkan_type;
	};

}
#endif //!VULKAN_TEXTURE_MANAGER_H