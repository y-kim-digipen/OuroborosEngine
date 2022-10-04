#ifndef TEXTURE_H
#define TEXTURE_H

#include "vulkan/vulkan_type.inl"
#include "texture.h"
#include "../common/assets.h"

namespace Asset
{
	struct Image;
}

namespace Renderer
{
	enum class E_ColorType
	{
		FLOAT,
		UNSIGNED_CHAR3,
		UNSIGNED_CHAR4,
	};

	class VulkanTexture
	{
	public:
		VulkanTexture(VulkanType* vulkan_type);
		~VulkanTexture();
		void Bind();
		void UploadData(const Asset::Image& data);
		void UpdateToDescripterSet(VkDescriptorSet descriptor_set, int dest_binding);
		void UpdateColorType(int channel);
		void Cleanup();
		VkDescriptorImageInfo GetImageInfo() const;
	private:
		VulkanType* vulkan_type = nullptr;
		uint32_t width_{ 0 };
		uint32_t height_{ 0 };

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


#endif !TEXTURE_H