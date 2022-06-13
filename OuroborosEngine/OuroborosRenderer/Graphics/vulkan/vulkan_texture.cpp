#include "vulkan_texture.h"

#include "vulkan_buffer.h"
#include "vulkan_image.h"
namespace Renderer
{
	VulkanTexture::VulkanTexture(Vulkan_type* vulkan_type) : vulkan_type(vulkan_type)
	{

	}

	void VulkanTexture::Bind()
	{


	}

	void VulkanTexture::UploadData(const Asset::Image& data)
	{
		Texture::UploadData(data);

		uint32_t width = data.width;
		uint32_t height = data.height;

		VkExtent3D image_extent
		{
			.width = width,
			.height = height,
			.depth = 1,
		};
		
		if(width != width_ || height != height_)
		{

			width_ = width;
			height_ = height;

			VkFilter filter = VK_FILTER_NEAREST;
			VkSamplerAddressMode sampler_address_mode = VK_SAMPLER_ADDRESS_MODE_REPEAT;

			VkSamplerCreateInfo sampler_create_info
			{
				.sType		  = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
				.magFilter	  = filter,
				.minFilter	  = filter,
				.addressModeU = sampler_address_mode,
				.addressModeV = sampler_address_mode,
				.addressModeW = sampler_address_mode,
			};

			vkCreateSampler(vulkan_type->device.handle, &sampler_create_info, nullptr, &sampler_);

			VkImageCreateInfo image_create_info
			{
				.sType			= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
				.imageType		= VK_IMAGE_TYPE_2D,
				.format			= VK_FORMAT_R8G8B8_SRGB,
				.extent			= image_extent,
				.mipLevels		= 1,
				.arrayLayers	= 1,
				.samples		= VK_SAMPLE_COUNT_1_BIT,
				.tiling			= VK_IMAGE_TILING_OPTIMAL,
				.usage			= VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT
			};

			VmaAllocationCreateInfo image_allocation_info{ .usage = VMA_MEMORY_USAGE_GPU_ONLY };

			VK_CHECK(vmaCreateImage(vulkan_type->allocator, &image_create_info, &image_allocation_info, &image_, &allocation_, nullptr));

			VkImageViewCreateInfo image_view_create_info
			{
				.sType		= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
				.image		= image_,
				.viewType   = VK_IMAGE_VIEW_TYPE_2D,
				.format		= VK_FORMAT_R8G8B8_SRGB,
				.subresourceRange = {
					.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT,
					.baseMipLevel	= 0,
					.levelCount		= 1,
					.baseArrayLayer = 0,
					.layerCount		= 1,
				}
			};

			VK_CHECK(vkCreateImageView(vulkan_type->device.handle, &image_view_create_info, nullptr, &image_view_));

			const uint32_t data_size = width_ * height_ * 4;

			const auto staging_buffer = std::make_shared<VulkanBuffer>(vulkan_type, data_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
			staging_buffer->UploadData(reinterpret_cast<int*>(data.image), data_size);

			VkImageSubresourceRange image_subresource_range
			{
				.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel   = 0,
				.levelCount     = 1,
				.baseArrayLayer = 0,
				.layerCount     = 1
			};


			VkImageMemoryBarrier imageMemoryBarrier{
				 .sType			   = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
				 .srcAccessMask    = 0,
				 .dstAccessMask    = VK_ACCESS_TRANSFER_WRITE_BIT,
				 .oldLayout		   = VK_IMAGE_LAYOUT_UNDEFINED,     // This may not necessarily be true if the texture already exists
			   	 .newLayout		   = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				 .image			   = image_,
				 .subresourceRange = image_subresource_range,
			};

			VkCommandBufferAllocateInfo allocate_info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
			allocate_info.level				  = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocate_info.commandPool		  = vulkan_type->command_pool;
			allocate_info.commandBufferCount  = 1;

			VkCommandBuffer command_buffer;
			vkAllocateCommandBuffers(vulkan_type->device.handle, &allocate_info, &command_buffer);


			//Record
			VkCommandBufferBeginInfo command_buffer_begin_info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
			command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
			vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info);

			//vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &)


		}



	}
}
