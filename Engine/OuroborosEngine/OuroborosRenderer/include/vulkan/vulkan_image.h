#ifndef VULKAN_IMAGE_H
#define VULKAN_IMAGE_H

#include "vulkan_type.inl"

struct VulkanType;
struct VulkanImage;

namespace Renderer {
    void CreateImage(
        VulkanType* vulkan_type,
        VulkanImage* out_image,
        VkImageType image_type,
        uint32_t width,
        uint32_t height,
        VkFormat format,
        VkImageUsageFlags usage_flags,
        VkImageTiling tiling,
        VkMemoryPropertyFlags memory_flags,
        bool create_view,
        VkImageAspectFlags aspect_flags,
        uint32_t mip_levels,
        uint32_t layer_count = 1
    );

    void CreateImageView(VulkanType* vulkan_type, VulkanImage* out_image, VkFormat format, VkImageAspectFlags aspect_flags, uint32_t layer_count);
    void DestroyImage(VulkanType* vulkan_type, VulkanImage* image);
}
#endif // !VULKAN_IMAGE_H

