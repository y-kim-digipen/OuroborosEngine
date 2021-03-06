#include "vulkan_image.h"

namespace Renderer {
    void CreateImage(Vulkan_type* vulkan_type, VulkanImage* out_image,
        VkImageType image_type,
        uint32_t width,
        uint32_t height,
        VkFormat format,
        VkImageUsageFlags usage_flags,
        VkImageTiling tiling,
        VkMemoryPropertyFlags memory_flags,
        bool create_view,
        VkImageAspectFlags aspect_flags
    )
    {
        VkImageCreateInfo create_info{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
        create_info.imageType = image_type;
        create_info.format = format;
        create_info.extent = { width, height, 1 };
        create_info.mipLevels = 4;
        create_info.arrayLayers = 1;
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
            CreateImageView(vulkan_type, out_image, format, aspect_flags);
        }
    }

    void CreateImageView(Vulkan_type* vulkan_type, VulkanImage* out_image, VkFormat format, VkImageAspectFlags aspect_flags)
    {
        VkImageViewCreateInfo create_info{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
        create_info.image = out_image->image;
        create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        create_info.format = format;
        create_info.subresourceRange.aspectMask = aspect_flags;
        create_info.subresourceRange.baseMipLevel = 0;
        create_info.subresourceRange.levelCount = 1;
        create_info.subresourceRange.baseArrayLayer = 0;
        create_info.subresourceRange.layerCount = 1;
        //VkComponentMapping         components;
       // VkImageSubresourceRange    subresourceRange;

        VK_CHECK(vkCreateImageView(vulkan_type->device.handle, &create_info, 0, &out_image->image_view));
    }

    void DestroyImage(Vulkan_type* vulkan_type, VulkanImage* image)
    {
        if (image->image != VK_NULL_HANDLE) {
            if (image->image_view != VK_NULL_HANDLE)
                vkDestroyImageView(vulkan_type->device.handle, image->image_view, 0);

            vkDestroyImage(vulkan_type->device.handle, image->image, 0);
        }
    }
}
