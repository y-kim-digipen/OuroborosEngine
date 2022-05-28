#ifndef VULKAN_IMAGE_H
#define VULKAN_IMAGE_H

#include "vulkan_type.inl"

struct Vulkan_type;
struct VulkanImage;

namespace Renderer {
    void CreateImage(
        Vulkan_type* vulkan_type,
        VulkanImage* out_image,
        VkImageType image_type,
        uint32_t width,
        uint32_t height,
        VkFormat format,
        VkImageUsageFlags usage_flags,
		VkImageTiling tiling,
        VkMemoryPropertyFlags memory_flags,
        bool create_view,
        VkImageAspectFlags aspect_flags
    );

	void CreateImageView(Vulkan_type* vulkan_type, VulkanImage* out_image, VkFormat format, VkImageAspectFlags aspect_flags);
    void DestroyImage(Vulkan_type* vulkan_type, VulkanImage* image);
}
#endif // !VULKAN_IMAGE_H
