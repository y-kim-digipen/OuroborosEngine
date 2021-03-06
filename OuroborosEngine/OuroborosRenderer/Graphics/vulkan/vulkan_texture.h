#ifndef VULKAN_TEXTURE_H
#define VULKAN_TEXTURE_H
#include "vulkan_type.inl"
#include "../texture.h"
#include "../common/assets.h"

namespace Renderer
{

	enum class E_ColorType
	{
		FLOAT,
		UNSIGNED_CHAR3,
		UNSIGNED_CHAR4,
	};

	class VulkanTexture : public Texture
	{
	public:
		VulkanTexture(Vulkan_type* vulkan_type);
		~VulkanTexture();
		void Bind() override;
		void UploadData(const Asset::Image& data) override;
		void UpdateToDescripterSet(VkDescriptorSet descriptor_set, int dest_binding);
		void UpdateColorType(int channel);
	private:
		Vulkan_type* vulkan_type = nullptr;
		uint32_t width_ {0};
		uint32_t height_ {0};

		VkImage image_;
		VmaAllocation allocation_;
		VkImageView image_view_;
		VkSampler sampler_;

		E_ColorType color_type = E_ColorType::UNSIGNED_CHAR4;
	};


	inline VkFormat ColorTypeToVulkanFormatType(E_ColorType colorType)
	{
		switch (colorType)
		{
		case E_ColorType::FLOAT:
			return VK_FORMAT_R32G32B32_SFLOAT;
		case E_ColorType::UNSIGNED_CHAR4:
			return VK_FORMAT_R8G8B8A8_SRGB;
		default:
			return VK_FORMAT_R8G8B8A8_SRGB;
		}
	}



}




#endif !VULKAN_TEXTURE_H
