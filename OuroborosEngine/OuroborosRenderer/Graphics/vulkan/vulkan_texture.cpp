#include <stdexcept>

#include "../texture.h"

#include "vulkan_buffer.h"
#include "vulkan_image.h"
namespace Renderer
{
	VulkanTexture::VulkanTexture(VulkanType* vulkan_type) : vulkan_type(vulkan_type)
	{

	}

	VulkanTexture::~VulkanTexture()
	{
		Cleanup();
	}

	void VulkanTexture::Bind()
	{
	}

	void VulkanTexture::UploadData(const Asset::Image& data)
	{

		uint32_t width = data.width;
		uint32_t height = data.height;

		VkExtent3D image_extent
		{
			.width = width,
			.height = height,
			.depth = 1,
		};
		UpdateColorType(data.channel);
		uint32_t mip_levels = 0;

		if (width != width_ || height != height_)
		{

			width_ = width;
			height_ = height;

			mip_levels = static_cast<uint32_t>(std::floor(std::log2(std::max(width_, height_)))) + 1;

			VkFilter filter = VK_FILTER_LINEAR;
			VkSamplerAddressMode sampler_address_mode = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			VkPhysicalDeviceProperties properties{};
			vkGetPhysicalDeviceProperties(vulkan_type->device.physical_device, &properties);


			VkSamplerCreateInfo sampler_create_info
			{
				.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
				.magFilter = filter,
				.minFilter = filter,
				.addressModeU = sampler_address_mode,
				.addressModeV = sampler_address_mode,
				.addressModeW = sampler_address_mode,
				.anisotropyEnable = VK_FALSE,
				.maxAnisotropy = properties.limits.maxSamplerAnisotropy,
				.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
				.unnormalizedCoordinates = VK_FALSE,

			};

			sampler_create_info.compareEnable = VK_FALSE;
			sampler_create_info.compareOp = VK_COMPARE_OP_ALWAYS;
			sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			sampler_create_info.minLod = 0.f;
			sampler_create_info.maxLod = static_cast<float>(mip_levels);
			sampler_create_info.mipLodBias = 0.2f;

			vkCreateSampler(vulkan_type->device.handle, &sampler_create_info, nullptr, &sampler_);


			VkFormat color_format = ColorTypeToVulkanFormatType(color_type);
			VkImageCreateInfo image_create_info
			{
				.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
				.imageType = VK_IMAGE_TYPE_2D,
				.format = color_format,
				.extent = image_extent,
				.mipLevels = mip_levels,
				.arrayLayers = 1,
				.samples = VK_SAMPLE_COUNT_1_BIT,
				.tiling = VK_IMAGE_TILING_OPTIMAL,
				.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
				.sharingMode = VK_SHARING_MODE_EXCLUSIVE
			};

			VmaAllocationCreateInfo image_allocation_info{ .usage = VMA_MEMORY_USAGE_GPU_ONLY };

			VK_CHECK(vmaCreateImage(vulkan_type->allocator, &image_create_info, &image_allocation_info, &image_, &allocation_, nullptr));

			VkImageViewCreateInfo image_view_create_info
			{
				.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
				.image = image_,
				.viewType = VK_IMAGE_VIEW_TYPE_2D,
				.format = color_format,
				.subresourceRange = {
					.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
					.baseMipLevel = 0,
					.levelCount = mip_levels,
					.baseArrayLayer = 0,
					.layerCount = 1,
				}
			};

			VK_CHECK(vkCreateImageView(vulkan_type->device.handle, &image_view_create_info, nullptr, &image_view_));
		}

		//TODO : need to fix the proper data type(jpg, png)

		const uint32_t data_size = width_ * height_ * 4;

		const auto staging_buffer = std::make_shared<VulkanBuffer>(vulkan_type, data_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
		staging_buffer->UploadData(reinterpret_cast<int*>(data.image), data_size);

		TransitionImageLayout(ColorTypeToVulkanFormatType(color_type), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mip_levels);
		VkCommandBufferAllocateInfo allocate_info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
		allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocate_info.commandPool = vulkan_type->command_pool;
		allocate_info.commandBufferCount = 1;

		VkCommandBuffer command_buffer;
		vkAllocateCommandBuffers(vulkan_type->device.handle, &allocate_info, &command_buffer);


		//Record
		VkCommandBufferBeginInfo command_buffer_begin_info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
			vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info);

		VulkanBuffer::CopyBufferToImage(command_buffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			staging_buffer.get(), &image_, image_extent);

		vkEndCommandBuffer(command_buffer);

		VkSubmitInfo submit_info{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &command_buffer;

		vkQueueSubmit(vulkan_type->device.graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
		vkQueueWaitIdle(vulkan_type->device.graphics_queue);

		vkFreeCommandBuffers(vulkan_type->device.handle, vulkan_type->command_pool, 1, &command_buffer);

		GenerateMipMaps(ColorTypeToVulkanFormatType(color_type), width_, height_, mip_levels);
	}

	void VulkanTexture::UpdateToDescripterSet(VkDescriptorSet descriptor_set, int dest_binding)
	{
		VkDescriptorImageInfo image_buffer_info
		{
				.sampler = sampler_,
				.imageView = image_view_,
				.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		};

		VkWriteDescriptorSet descriptor_set_write{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
		descriptor_set_write.dstSet = descriptor_set;
		descriptor_set_write.dstBinding = dest_binding;
		descriptor_set_write.dstArrayElement = 0;
		descriptor_set_write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptor_set_write.descriptorCount = 1;
		descriptor_set_write.pImageInfo = &image_buffer_info;

		vkUpdateDescriptorSets(vulkan_type->device.handle, 1, &descriptor_set_write, 0, nullptr);
	}

	void VulkanTexture::UpdateColorType(int channel)
	{
		switch (channel)
		{
			case 1:
				color_type = E_ColorType::UNSIGNED_CHAR4;
				break;
			default:
				color_type = E_ColorType::UNSIGNED_CHAR3;
		}
	}

	void VulkanTexture::Cleanup()
	{
		if (vulkan_type->device.handle != VK_NULL_HANDLE) {
			vkDestroySampler(vulkan_type->device.handle, sampler_, nullptr);
			vmaDestroyImage(vulkan_type->allocator, image_, allocation_);
			vkDestroyImageView(vulkan_type->device.handle, image_view_, nullptr);
		}
	}

	void VulkanTexture::TransitionImageLayout(VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout,
		uint32_t mip_levels)
	{
		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = old_layout;
		barrier.newLayout = new_layout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image_;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = mip_levels;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else {
			throw std::invalid_argument("unsupported layout transition!");
		}
		VkCommandBufferAllocateInfo allocate_info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
		allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocate_info.commandPool = vulkan_type->command_pool;
		allocate_info.commandBufferCount = 1;

		VkCommandBuffer command_buffer;
		vkAllocateCommandBuffers(vulkan_type->device.handle, &allocate_info, &command_buffer);


		//Record
		VkCommandBufferBeginInfo command_buffer_begin_info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
			vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info);


		vkCmdPipelineBarrier(
			command_buffer,
			sourceStage, destinationStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		vkEndCommandBuffer(command_buffer);

		VkSubmitInfo submit_info{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &command_buffer;

		vkQueueSubmit(vulkan_type->device.graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
		vkQueueWaitIdle(vulkan_type->device.graphics_queue);

		vkFreeCommandBuffers(vulkan_type->device.handle, vulkan_type->command_pool, 1, &command_buffer);
	}

	void VulkanTexture::GenerateMipMaps(VkFormat image_format, int32_t tex_width, int32_t tex_height,
	                                    uint32_t mip_levels)
	{
		VkFormatProperties format_properties;
		vkGetPhysicalDeviceFormatProperties(vulkan_type->device.physical_device, image_format, &format_properties);

		if(!(format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
		{
			throw std::runtime_error("texture image format do not support linear blitting");
		}

		VkCommandBufferAllocateInfo allocate_info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
		allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocate_info.commandPool = vulkan_type->command_pool;
		allocate_info.commandBufferCount = 1;

		VkCommandBuffer command_buffer;
		vkAllocateCommandBuffers(vulkan_type->device.handle, &allocate_info, &command_buffer);

		//Record one time command buffer
		VkCommandBufferBeginInfo command_buffer_begin_info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
			vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info);


		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.image = image_;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.subresourceRange.levelCount = 1;
		
		int32_t mip_width = tex_width;
		int32_t mip_height = tex_height;

		for(uint32_t idx = 1; idx < mip_levels; idx++)
		{
			barrier.subresourceRange.baseMipLevel = idx - 1;
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

			VkImageBlit blit{};
			blit.srcOffsets[0] = { 0,0,0 };
			blit.srcOffsets[1] = { mip_width,mip_height,1 };
			blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.srcSubresource.mipLevel = idx - 1;
			blit.srcSubresource.baseArrayLayer = 0;
			blit.srcSubresource.layerCount = 1;

			blit.dstOffsets[0] = { 0,0,0 };
			blit.dstOffsets[1] = {mip_width > 1 ? mip_width/ 2 : 1, mip_height > 1 ? mip_height /2 : 1, 1};
			blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.dstSubresource.mipLevel = idx;
			blit.dstSubresource.baseArrayLayer = 0;
			blit.dstSubresource.layerCount = 1;

			vkCmdBlitImage(command_buffer, image_, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image_, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);

			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

			if (mip_width > 1)
			{
				mip_width /= 2;
			}
			if (mip_height > 1)
			{
				mip_height /= 2;
			}
		}

		barrier.subresourceRange.baseMipLevel = mip_levels - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

		vkEndCommandBuffer(command_buffer);

		VkSubmitInfo submit_info{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &command_buffer;

		vkQueueSubmit(vulkan_type->device.graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
		vkQueueWaitIdle(vulkan_type->device.graphics_queue);

		vkFreeCommandBuffers(vulkan_type->device.handle, vulkan_type->command_pool, 1, &command_buffer);

	}

	VkDescriptorImageInfo VulkanTexture::GetImageInfo() const
	{
		return VkDescriptorImageInfo{
			sampler_,
			image_view_,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		};
	}
}
