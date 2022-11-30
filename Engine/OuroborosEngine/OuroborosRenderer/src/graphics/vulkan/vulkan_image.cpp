#include "vulkan_image.h"

namespace Renderer {
    void CreateImage(VulkanType* vulkan_type, VulkanImage* out_image,
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
        uint32_t layer_count
    )
    {
        VkImageCreateInfo create_info{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
        create_info.imageType = image_type;
        create_info.format = format;
        create_info.extent = { width, height, 1 };
        create_info.mipLevels = mip_levels;
        create_info.arrayLayers = layer_count;
        create_info.samples = VK_SAMPLE_COUNT_1_BIT;
        create_info.tiling = tiling;
        create_info.usage = usage_flags;
        create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        //create_info.queueFamilyIndexCount;
        //create_info.pQueueFamilyIndices;
        create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        VmaAllocationCreateInfo alloc_create_info{};
        alloc_create_info.usage = VMA_MEMORY_USAGE_AUTO;
        alloc_create_info.requiredFlags = memory_flags;

        VK_CHECK(vmaCreateImage(vulkan_type->allocator, &create_info, &alloc_create_info, &out_image->image, &out_image->allocation, 0));

        if (create_view) {
            CreateImageView(vulkan_type, out_image, format, aspect_flags, layer_count);
        }
    }

    void CreateImageView(VulkanType* vulkan_type, VulkanImage* out_image, VkFormat format, VkImageAspectFlags aspect_flags,uint32_t layer_count)
    {
        VkImageViewCreateInfo create_info{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
        create_info.image = out_image->image;
        create_info.viewType = (layer_count == 1) ? VK_IMAGE_VIEW_TYPE_2D : VK_IMAGE_VIEW_TYPE_2D_ARRAY;
        create_info.format = format;
        create_info.subresourceRange.aspectMask = aspect_flags;
        create_info.subresourceRange.baseMipLevel = 0;
        create_info.subresourceRange.levelCount = 1;
        create_info.subresourceRange.baseArrayLayer = 0;
        create_info.subresourceRange.layerCount = layer_count;
        //VkComponentMapping         components;
       // VkImageSubresourceRange    subresourceRange;

        VK_CHECK(vkCreateImageView(vulkan_type->device.handle, &create_info, 0, &out_image->image_view));
    }

    void DestroyImage(VulkanType* vulkan_type, VulkanImage* image)
    {
        if (image->image != VK_NULL_HANDLE) {
            if (image->image_view != VK_NULL_HANDLE)
                vkDestroyImageView(vulkan_type->device.handle, image->image_view, 0);

            vmaDestroyImage(vulkan_type->allocator, image->image, image->allocation);
        }
    }
}
