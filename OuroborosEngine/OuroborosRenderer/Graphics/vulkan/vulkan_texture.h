#ifndef VULKAN_TEXTURE_H
#define VULKAN_TEXTURE_H
#include "vulkan_type.inl"
#include "../texture.h"
namespace Renderer
{
	class VulkanTexture : public Texture
	{
	public:
		VulkanTexture(Vulkan_type* vulkan_type);
		void Bind() override;
		void UploadData(const Asset::Image& data) override;


	private:
		Vulkan_type* vulkan_type = nullptr;
		uint32_t width_ {0};
		uint32_t height_ {0};

		VkImage image_;
		VmaAllocation allocation_;
		VkImageView image_view_;
		VkSampler sampler_;
	};


}




#endif !VULKAN_TEXTURE_H
